
#define TIME_LIMIT ....

#define TOP 0
#define BOTTOM 1

//input
double mean_t_inter_arrival = 1/59.94;
double C_enc = 15800(fobs/sec);
double C_stor = 1600 (fobs/sec);
double alpha = 0.1;
double field_complexity = 265.2;
int max_queue_length = ....(beta);

//environment
int encoder_buff_length = 0;
bool encoder_busy = false;
bool fieldtype_encoder_front = TOP;
bool fieldtype_encoder_back = TOP;
bool this_arrival_type;
bool field_type_encoder_served;

int storage_buff_length = 0;
enum event_type {ARRIVAL, ENCODER_FINISH, STORAGE_FINISH};


//statistic
int count_lost_field = 0;

main(){

    encoder_busy = false;
    this_arrival_type = TOP
    creat first ARRIVAL event with type TOP;

    while(current_time < TIME_LIMIT){
        取出 this_event from e_list
        current_time = this_event->event_time;

        switch(this_event->EventType){
            case ARRIVAL:
            {
                // no field in the encoder
                if (encoder_busy == false) {
                    產生field fob
                    encoder_busy = true;
                    field_type_encoder_served = this_arrival_type;
                    產生ENCODER_FINISH event
                    this_arrival_type = !this_arrival_type;
                }
                else if (encoder_buff_length < max_queue_length) {
                    if(encoder_buff_length == 0)
                    {
                        encoder_buff_length++;
                        fieldtype_encoder_back = fieldtype_encoder_front = this_arrival_type;
                        this_arrival_type = !this_arrival_type;
                    }
                    else{
                        encoder_buf_length++;
                        fieldtype_encoder_back = this_arrival_type;
                        this_arrival_type = !this_arrival_type;
                    }
                }
                else{
                    if(this_arrival_type == TOP){
                        count_lost_field+=2;
                        產生兩次next_arrival_time並相加，當成下一次arrival_time (跳過一次bottom的arrival)
                    }
                    else{
                        count_lost_field+=2;
                        encoder_buff_length--;
                        this_arrival_type = !this_arrival_type;
                        fieldtype_encoder_back = !fieldtype_encoder_back;

                        產生一個next_arrival_time，
                    }
                }
                產生next ARRIVAL event;

                break;
            }
            case ENCODER_FINISH:{
                storage_buff_length++;

                if(encoder_buf_length != 0){
                    field_type_encoder_served = fieldtype_encoder_front;
                    encoder_buf_length--;
                    fieldtype_encoder_front = !fieldtype_encoder_front;

                    creat next ENCODER_FINISH event;
                }
                else{
                    encoder_busy = false;
                }
                break;
            }
            
            case STORAGE_FINISH:
            {

                break;
            }
            
        }

    }
}


void creatNextEvent(double time, int eventType){
    set各種參數
    塞到e_list裡
}

