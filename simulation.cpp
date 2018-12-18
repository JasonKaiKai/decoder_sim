#include <iostream>
#include "e_list.h"
#include "event.h"
#include "random.h"

#define TIME_LIMIT (8 * 60 * 60)
#define TOP 0
#define BOTTOM 1
#define C_ENC 15800
#define C_STORAGE 1600
#define ALPHA 0.1
using namespace std;

// input
double rate_inter_arrival; // tau
double rate_field_complexity;    //h
int max_encoder_queue_length;   //(beta)


// environment
int encoder_buff_length = 0;
bool encoder_busy = false;
bool fieldType_encoder_front = TOP;
bool fieldType_encoder_back = TOP;
bool fieldType_encoder_served;
bool this_arrival_type;

int storage_buff_length = 0;

enum event_type {ARRIVAL, ENCODER_FINISH, STORAGE_FINISH};
double current_time = 0.1;

// random variable
Expon rv_fobs_of_field;
Expon rv_inter_arrival_t;

// statistic
int count_lost_field = 0;

//prototype
int CreateEvent(E_List*, int, double);
int CreateEventwithType(E_List*, int, double, int);


int main(int argc, char** argv) {
    rate_inter_arrival = strtod(argv[1], NULL);
    rate_field_complexity = strtod(argv[2], NULL);
    max_encoder_queue_length = strtod(argv[3], NULL);

    rv_fobs_of_field.SetMean(1/rate_field_complexity);
    rv_inter_arrival_t.SetMean(1/rate_inter_arrival);

    encoder_busy = false;
    this_arrival_type = TOP;
    
    Event* this_event;
    E_List* E_List_ptr = new E_List;
   
    // Create the first ARRIVAL event with type TOP
    CreateEvent(E_List_ptr, ARRIVAL, current_time+rv_inter_arrival_t++);
    
    while (current_time < TIME_LIMIT) {
        // 取出 this_event from e_list
        *E_List_ptr >> this_event;
        current_time = this_event -> getTimeStamp();

        switch(this_event -> getEventType()) {
            case ARRIVAL: {
                double next_arrival_time = current_time + rv_inter_arrival_t++;

                if (encoder_busy == false) {
                    encoder_busy = true;
                    fieldType_encoder_served = this_arrival_type;

                    double time_encode = rv_fobs_of_field++ / C_ENC;
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
                        this_arrival_type = !this_arrival_type;
                        fieldType_encoder_back = !fieldType_encoder_back;
                    }
                }
                CreateEvent(E_List_ptr, ARRIVAL, next_arrival_time);
                break;
            }
            case ENCODER_FINISH: {

                break;
            }
            case STORAGE_FINISH: {
                break;
            }
        }
    }

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