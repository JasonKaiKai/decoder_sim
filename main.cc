#include <iostream>
#include <fstream>
#include "e_list.h"
#include "event.h"
#include "random.h"
#include "mydef.h"



#define Idea_Arr 1 
#define Paper_Sub 2

int Iteration = 1000000;

int timeSet[9] = {0, 5, 10, 20, 60, 180, 360, 720, 1440} ;
int sizeOftimeSet = 9 ;

float mean_idea_arr ;
float mean_paper_sub ;

double lambda_idea_arr = 3;
double lambda_paper_sub = 7;
int num_of_srv=2;

double currentTime = 0 ;
double waitingTime = 0 ; // sum of waiting time
double serviceTime = 0 ; // sum of service time
double T=0;              // waiting time + service time
double sumT = 0;         //all  success event waiting time + service time

long int ND = 0 ;  // number of ideas in queue seen by departures
int NF = 0 ;       // number of total idea departure
int NQ = 0 ;       // number of ideas in queue
int NW = 0 ;       // number of idea currently being written (0 or 1)
long int NS = 0 ;  // number of ideas in queue seen by arrivals
int NI = 0 ;       // number of total idea arrivals
int NT = 0 ;       // number of [T<=2] idea
int NU = 1 ;       // number of busy server

char *p;
double temp;

int CreateEvent(E_List *ptr, int type, double ts){

	Event *event;
	event=new Event();
	event->setEventType(type);
	event->setTimestamp(ts);
	*ptr<<*event;
    
    return 1 ;
}

int CreateEvent(E_List *ptr, int type, double ts, int timeSlot){
    
	Event *event;
	event=new Event();
	event->setEventType(type);
	event->setTimestamp(ts);
    	event->setTimeSlot(timeSlot);
	*ptr<<*event;
    
    return 1 ;
}


int main(int argc, char ** argv) {
    
    mean_idea_arr =(float) 1 / lambda_idea_arr ;
    mean_paper_sub=(float) 1 / lambda_paper_sub;
    
    printf("Iteration = %d , Number of server = %d\n",Iteration,num_of_srv); 
    printf("lambda_idea_arr = %f , lambda_paper_sub = %f\n",lambda_idea_arr,lambda_paper_sub);	    
    printf("------------- result --------------\n");	

    Expon t_ideaarr(mean_idea_arr);
    Expon t_papersub(mean_paper_sub);
    
    double t[1000000];  //caculate each event waiting time
    	
//    double serviceStartTime=0; //service time		 
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
    CreateEvent(E_List_ptr, Idea_Arr, t_ideaarr++,1);
    		
    
    while (NI < Iteration) {
        
		*E_List_ptr  >> FirstEventofEList;       
        currentTime = FirstEventofEList -> getTimeStamp();
      	
        switch(FirstEventofEList -> getEventType()) {
            case Idea_Arr:
                t[NI]=currentTime;
				NS+=NQ;
				NI++;	
                CreateEvent(E_List_ptr, Idea_Arr, (currentTime + t_ideaarr++));
				if(NW>0)
		    		NU++;               	

				if(NW==num_of_srv)
		    		NQ++;	 
                else
				{	
		    		ND+=NQ;
		    		NW++;
                    waitingTime+=currentTime-t[NF];
		   			T=0;
		    		T+=currentTime-t[NF];
		    		NF++;	
		    		temp=t_papersub++;
		    		CreateEvent(E_List_ptr, Paper_Sub, (currentTime + temp));
		    		serviceTime+=temp;	
		    		T+=temp;
		    		sumT+=T;
                    if((T<2)||(T==2))
						NT++;
				}
			break;
            
	     	case Paper_Sub:
				if(NQ>0)
				{
		    		NQ--;
		    		ND+=NQ;
		    		waitingTime+=currentTime-t[NF];
                    T=0;
		    		T+=currentTime-t[NF];
		    		NF++;
		    		temp=t_papersub++;
                    CreateEvent(E_List_ptr, Paper_Sub, (currentTime + temp));
		    		serviceTime+=temp;
		    		T+=temp;
		    		sumT+=T;
		    		if((T<2)||(T==2))
                        NT++; 
	        	}
				else
				{
		    		if(NW>0)
			   	 	NW--;
				}		
                break ;
                
            default:
                break;
        }
	delete FirstEventofEList;
    }
	printf("Mean queue length seen by an arrival =%f\n",((float)NS/(float)NI) );
	printf("Mean queue length seen by a departure =%f\n",((float)ND/(float)NI) );
	printf("Mean waiting time of a customer =%f\n",(waitingTime/NF));
	printf("Mean service time of a customer =%f\n",(serviceTime/NF));
	printf("Utilization of the server =%f\n",(float)NU/(float)NI);
	printf("Mean T =%f\n",(float)sumT/(float)NF);
	printf("Pr[T<=2]=%f\n",(float)NT/(float)NF);
}
