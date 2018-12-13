#include <iostream>
#include <fstream>
#include "e_list.h"
#include "event.h"
#include "random.h"
#include "mydef.h"



#define DISCONNECT 1 
#define CONNECT 2
#define PACKET_ARRIVAL 3
#define PACKET_RETRANSMISSION 4

int Iteration = 10000000 ;

double currentTime = 0 ; //the time stamp

int timeSet[9] = {0, 5, 5, 10, 40, 120, 180, 360, 720} ;
int sizeOftimeSet = 9 ;

float mean_disconnect = 1.0 ;
float mean_connect = 1.0 ;
float mean_packet_arrival = 1.0 ;
float mean_retransmission = 1.0 ;

double lambda = 1.0 ;
double lambda_disconnect = 1.0 ;
double lambda_connect = 1.0 ;
double lambda_packet = 1.0 ;
double lambda_retransmission = 1.0 ;

bool isConnect = true ;

long numOfFinish = 0 ;
long numOfFail = 0 ;
double numOfRetransmission = 0 ;
long numOfPacket = 0 ;

double numOfDelayTime = 0 ;

char *p;
double arrival_timestamp = 0 ;

// For connect, disconnect event
int CreateEvent(E_List *ptr, int type, double ts){

	Event *event;
	event=new Event();
	event->setEventType(type);
	event->setTimestamp(ts);
	*ptr<<*event;
    
    return 1 ;
}

// For arrival and retransmission event
int CreateEvent(E_List *ptr, int type, double ts, double st, int delayIndex){
    
	Event *event;
	event=new Event();
	event->setEventType(type);
	event->setTimestamp(ts);
    event->setStartTime(st);
    event->setCurrentDelayTimeIndex(delayIndex);
	*ptr<<*event;
    
    return 1 ;
}

// For arrival and retransmission event
/*int CreateEvent(E_List *ptr, int type, double ts, double dt, double st, int delayIndex){
    
	Event *event;
	event=new Event();
	event->setEventType(type);
	event->setTimestamp(ts);
    event->setDelayTime(dt);
    event->setStartTime(st);
    event->setCurrentDelayTimeIndex(delayIndex);
	*ptr<<*event;

    return 1 ;
}*/

double getNextTransmissionTime(int index){

    return timeSet[index+1];
    
}


int main(int argc, char ** argv) {
    //printf("hello \n");
    
    if(argc != 3){
        printf("command : ./main test_time lambda_times_of_mcd \n");
        return 0;
    }
    
    Iteration = strtol(argv[1], &p, 10);
    lambda = strtod(argv[2], &p);
    
    mean_packet_arrival = lambda ;
    mean_retransmission = lambda ;
    mean_connect = 1 ;
    mean_disconnect = mean_connect ;
    
    //mean_packet_arrival = 1/(strtod(argv[2], &p)*mean_connect);
    
    printf("Iteration = %d , lambda = %f, mean_connect = %f, mean_disconnect = %f, mean_packet_arrival = %f, mean_retransmission = %f\n", Iteration,lambda,mean_connect,mean_disconnect,mean_packet_arrival,mean_retransmission) ;
    
    Expon t_disconnect(mean_disconnect);
    Expon t_connect(mean_connect);
    Expon t_packet(mean_packet_arrival);
    Expon t_retransmission(mean_retransmission);
    
    /*
     *  Generate the first event and insert it into the event list
     */
    E_List* E_List_ptr;
    Event * FirstEventofEList;
    Event * EventNode;
    E_List_ptr  = new E_List;
    
    
    /*
     * Create 1st event
     */
    CreateEvent(E_List_ptr, DISCONNECT, t_connect++);
    
    arrival_timestamp = currentTime + t_packet++ ; //set the arrival timestamp
    CreateEvent(E_List_ptr, PACKET_ARRIVAL, arrival_timestamp, arrival_timestamp, 0);

    while (numOfFinish < Iteration) {
        
        *E_List_ptr  >> FirstEventofEList;
        
        currentTime = FirstEventofEList -> getTimeStamp();
        
        switch(FirstEventofEList -> getEventType()) {
            case CONNECT:
                
                //printf("CONNECT currentTime = %f\n",currentTime);
                
                CreateEvent(E_List_ptr, DISCONNECT, (currentTime + t_connect++));
                isConnect = true ;
                break;
                
            case DISCONNECT:
                
                //printf("DISCONNECT currentTime = %f\n",currentTime);
                
                CreateEvent(E_List_ptr, CONNECT, (currentTime + t_disconnect++));
                isConnect = false ;
                break ;
                
            case PACKET_ARRIVAL :
                
                //printf("PACKET_ARRIVAL \n");
                
                if (isConnect) {
                    numOfFinish++ ;
                }else{
                    double nextTransmissionTime = currentTime + t_disconnect++;
                    
                    //can't send, create a retransmission event
                    CreateEvent(E_List_ptr,
                                PACKET_RETRANSMISSION,
                                nextTransmissionTime,
                                currentTime,
                                1);
                }

                
                //create another arrival event
                arrival_timestamp = currentTime + t_packet++ ; //set the arrival timestamp
                CreateEvent(E_List_ptr,
                            PACKET_ARRIVAL,
                            arrival_timestamp,
                            arrival_timestamp,
                            0);

                break;
            case PACKET_RETRANSMISSION :
                
                //printf("PACKET_RETRANSMISSION : isConnect %d , timeStamp %f , timeslot %d \n", isConnect, FirstEventofEList->getTimeStamp(), timeSet[FirstEventofEList->getTimeSlot()]);
                
                
                
                if(isConnect){
                    numOfFinish++ ;
                    numOfDelayTime = numOfDelayTime + currentTime - (FirstEventofEList ->getStartTime());
                    numOfRetransmission = numOfRetransmission + FirstEventofEList ->getCurrentDelayTimeIndex();
                    
                }else{
                
                    if(FirstEventofEList ->getCurrentDelayTimeIndex() == sizeOftimeSet){
                        numOfFail ++ ;
                        numOfRetransmission = numOfRetransmission + sizeOftimeSet;
                        
                        
                    }else{
                        double nextTransmissionTime = currentTime + t_retransmission++ ;
                        
                        //can't send, create a retransmission event
                        CreateEvent(E_List_ptr,
                                    PACKET_RETRANSMISSION,
                                    nextTransmissionTime,
                                    FirstEventofEList->getStartTime(),
                                    FirstEventofEList ->getCurrentDelayTimeIndex()+1);
                        
                    }
                    
                }
                break ;
                
            default:
                break;
        }
        delete FirstEventofEList;
    }
    
    printf("NumOfFinish = %ld \n", numOfFinish);
    printf("numOfRetransmission = %f \n", numOfRetransmission);
    printf("NumOfFail = %ld \n", numOfFail);
    printf("number of retransmission = %f \n", (float)(numOfRetransmission*10000/numOfFinish)/10000 );
    //printf("timePeriod = %f \n",timePeriod);
    //printf("timePeriod = %f \n",timePeriod/numOfFinish);
    //printf("timeretransmission = %f \n",(float)(timePeriod*10000/numOfRetransmission)/10000);
    printf("Success Rate = %f \n", (float)(numOfFinish*10000 / (numOfFinish + numOfFail))/10000);
    printf("T_S = %f \n", numOfDelayTime);
    printf("T_S = %f \n", (double)(numOfDelayTime/numOfFinish));
}
