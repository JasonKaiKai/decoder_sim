#include <iostream>
#include "e_list.h"
#include "event.h"
#include "random.h"

#define TIME_LIMIT (8 * 60 * 60)
#define TOP 0
#define BOTTOM 1
#define tau         (1/59.94)
#define h           262.5

// input
double mean_inter_arrival = tau;
int C_enc = 15800;
int C_storage = 1600;
double alpha = 0.1;
int beta = 20;
double field_complexity = h;
int max_queue_length = beta;

// environment
int encoder_buff_length = 0;
bool encoder_busy = false;
bool fieldType_encoder_front = TOP;
bool fieldType_encoder_back = TOP;
bool fieldType_encoder_served;
bool this_arrival_type;

int storage_buff_length = 0;

enum event_type {ARRIVAL, ENCODER_FINISH, STORAGE_FINISH};
double current_time = 0;

// statistic
int count_lost_field = 0;

using namespace std;

int CreateEvent(E_List*, int, double);
int CreateEventwithType(E_List*, int, double, int);
int main() {

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
                // no field in the encoder
                if (encoder_busy == false) {
                    // To-Do: 產生 field fob
                    encoder_busy = true;
                    fieldType_encoder_served = this_arrival_type;
                    // To-Do: 產生 ENCODER_FINISH event
                    this_arrival_type = !this_arrival_type;
                }
                else if (encoder_buff_length < max_queue_length) {
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
                        // To-Do: 產生兩次 next_arrival_time 並相加，當成下一次 arrival_time (跳過一次 bottom 的 arrival)
                    }
                    else {
                        count_lost_field += 2;
                        encoder_buff_length --;
                        this_arrival_type = !this_arrival_type;
                        fieldType_encoder_back = !fieldType_encoder_back;

                        // 產生一個 next_arrival_time
                    }
                }
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