#include <iostream>
#include <fstream>
#include <queue>
#include "e_list.h"
#include "event.h"
#include "random.h"

#define TIME_LIMIT (8*60*60)
#define TOP 0
#define BOTTOM 1
#define C_ENC 15800
#define C_STORAGE 1600
#define ALPHA 0.1

#define DEBUG false
using namespace std;

// input
double rate_inter_arrival; // tau
double mean_field_complexity;    //h
int max_encoder_queue_length;   //(beta)


// environment
int encoder_buff_length = 0;
bool encoder_busy = false;
bool fieldType_encoder_front = TOP;
bool fieldType_encoder_back = TOP;
bool fieldType_encoder_served;
bool this_arrival_type;

int storage_buff_length = 0;
bool storage_busy = false;
queue<double> q_field_size;

enum event_type {ARRIVAL, ENCODER_FINISH, STORAGE_FINISH};
double current_time = 0.1;

fstream fp;
fstream fp_result;

// random variable
Expon rv_fobs_of_field;
Expon rv_inter_arrival_t;

// statistic
int count_field_arrival = 0;
int count_lost_field = 0;

double t_last_storage_change_state=0;
double storage_busy_time=0;

//prototype
int CreateEvent(E_List*, int, double);
int CreateEventwithType(E_List*, int, double, int);
void staStorageBusyT(double, bool);
void printState();

int main(int argc, char** argv) {
    rate_inter_arrival = strtod(argv[1], NULL);
    mean_field_complexity = strtod(argv[2], NULL);
    max_encoder_queue_length = strtod(argv[3], NULL);

    rv_fobs_of_field.SetMean(mean_field_complexity);
    rv_inter_arrival_t.SetMean(1/rate_inter_arrival);

    fp.open("debug.txt", ios::out);    
    fp_result.open("result.txt", ios::app);

    encoder_busy = false;
    this_arrival_type = TOP;
    
    Event* this_event;
    E_List* E_List_ptr = new E_List;
   
    // Create the first ARRIVAL event with type TOP
    CreateEvent(E_List_ptr, ARRIVAL, current_time);
    
    while (current_time < TIME_LIMIT) {
        // 取出 this_event from e_list
        *E_List_ptr >> this_event;
        current_time = this_event -> getTimeStamp();

        switch(this_event -> getEventType()) {
            case ARRIVAL: {
                double next_arrival_time = current_time + rv_inter_arrival_t++;

                if (encoder_busy == false) {
                    double field_size = rv_fobs_of_field++;
                    double time_encode = field_size / C_ENC;
                    
                    encoder_busy = true;
                    fieldType_encoder_served = this_arrival_type;
                    q_field_size.push(field_size);

                    CreateEvent(E_List_ptr, ENCODER_FINISH, current_time+time_encode);
                    this_arrival_type = !this_arrival_type;
                }
                else if (encoder_buff_length < max_encoder_queue_length) {
                    if (encoder_buff_length == 0) {
                        encoder_buff_length++;
                        fieldType_encoder_front = this_arrival_type;
                        fieldType_encoder_back = this_arrival_type;
                        this_arrival_type = !this_arrival_type;
                    }
                    else {
                        encoder_buff_length++;
                        fieldType_encoder_back = this_arrival_type;
                        this_arrival_type = !this_arrival_type;
                    }
                }
                else {
                    if(this_arrival_type == TOP) {
                        count_lost_field += 2;
                        next_arrival_time = next_arrival_time + rv_inter_arrival_t++;
                    }
                    else {
                        count_lost_field += 2;
                        encoder_buff_length --;
                        fieldType_encoder_back = !fieldType_encoder_back;
                        this_arrival_type = !this_arrival_type;
                    }
                }
                CreateEvent(E_List_ptr, ARRIVAL, next_arrival_time);

                count_field_arrival++;

                if(DEBUG){
                    fp<<"ARRIVAL"<<endl;
                    printState();
                }

                break;
            }
            case ENCODER_FINISH: {
                if(encoder_buff_length!=0){
                    fieldType_encoder_served = fieldType_encoder_front;
                    encoder_buff_length--;
                    fieldType_encoder_front = !fieldType_encoder_front;

                    double field_size = rv_fobs_of_field++;
                    double time_encode = field_size / C_ENC;
                    q_field_size.push(field_size);
                    CreateEvent(E_List_ptr, ENCODER_FINISH, current_time+time_encode);     
                }
                else{
                    encoder_busy = false;
                }

                if(storage_buff_length==1 && storage_busy==false){
                    storage_buff_length -= 1;
                    storage_busy = true;
                    
                    //calculate storage processing time
                    double field_size_first = q_field_size.front();
                    q_field_size.pop();
                    double field_size_second = q_field_size.front();
                    q_field_size.pop();
                    double frame_size = ALPHA*(field_size_first+field_size_second);
                    double time_storage_process = frame_size/C_STORAGE;
                    
                    staStorageBusyT(current_time, storage_busy);
                    CreateEvent(E_List_ptr, STORAGE_FINISH, current_time+time_storage_process);                    
                }
                else{
                    storage_buff_length += 1;
                }

                if(DEBUG){
                    fp<<"ENCODE FINISH"<<endl;
                    printState();
                }
                break;
            }
            case STORAGE_FINISH: {
                if(storage_buff_length<=1){
                    storage_busy = false;
                    staStorageBusyT(current_time, storage_busy);
                }
                else{
                    storage_buff_length -= 2;

                    double field_size_first = q_field_size.front();
                    q_field_size.pop();
                    double field_size_second = q_field_size.front();
                    q_field_size.pop();
                    double frame_size = ALPHA*(field_size_first+field_size_second);
                    double time_storage_process = frame_size/C_STORAGE;
                    CreateEvent(E_List_ptr, STORAGE_FINISH, current_time+time_storage_process);
                }

                if(DEBUG){
                    fp<<"STORAGE FINISH"<<endl;
                    printState();
                }
                break;
            }
        }
    }

    cout<< "blocking prob: "<< (double)count_lost_field/count_field_arrival<<endl;
    cout<< "Storage utilization: "<<(double)storage_busy_time / current_time<<endl;
    fp_result<<(double)count_lost_field/count_field_arrival<<" "
            <<(double)storage_busy_time / current_time<<endl;

    fp.close();
    fp_result.close();

    return 0;
}

int CreateEvent(E_List* ptr, int type, double timestamp) {
    Event* event = new Event();
    event -> setEventType(type);
    event -> setTimestamp(timestamp);
    *ptr << *event;

    return 1;
}
int CreateEventwithType (E_List* ptr, int type, double timestamp, int fieldtype) {
    Event* event = new Event();
    event -> setEventType(type);
    event -> setFieldType(fieldtype);
    event -> setTimestamp(timestamp);
    *ptr << *event;

    return 1;
}

void staStorageBusyT(double currentTime, bool stateAfterChange){
    if(stateAfterChange==false){
        storage_busy_time += (currentTime - t_last_storage_change_state);
    }
    t_last_storage_change_state = current_time;
}
 
void printState(){
    fp<<"current time: "<<current_time<<endl
        <<"encoder buff len: "<<encoder_buff_length<<endl
        <<"encoder busy: "<<encoder_busy<<endl
        <<"fieldType_encoder_front: "<<fieldType_encoder_front<<endl
        <<"fieldType_encoder_back: "<<fieldType_encoder_back<<endl
        <<"fieldType_encoder_served: "<<fieldType_encoder_served<<endl
        <<"storage buff length: "<<storage_buff_length<<endl
        <<"storage busy: "<<storage_busy<<endl
        <<"---------"<<endl
        <<"queue size: "<<q_field_size.size()<<endl
        <<"front: "<<q_field_size.front()<<endl
        <<"back: "<<q_field_size.back()<<endl<<endl
        <<"-------------------------------------------------------------"<<endl;

}


