#include        "e_list.h"
using namespace std; 
#define         CHUNK           (30)    /* number of event list per     */
                                        /*  system allocation           */
E_List* E_List::freeList = NULL;        /* initial free list to empty   */
            
void E_List::operator<<(Event& node) {
 Event* ptr;                            /* pointer to the node in list  */
 Event* prev;                           /* prev points to the previous  */
                                        /*  node of which ptr pointed   */
 double ts;                             /* time stamp of the input node */
         
        ptr = prev = head;              /* set ptr, prev to head node   */
        if((ts  = node.getTimeStamp())>0) {
        	while ((ptr=ptr->getNext())  != NULL) {/* while next not NULL  */
                	if (ptr->getTimeStamp() <= ts) {    
                	prev=ptr;
                          
                        }
            		else break;
                }
        	node.insNext( ptr);/* this is new method not copy*/
                prev->insNext(&node); /* just do the insert*/
        }
}
ostream& operator<<( ostream& os, E_List& list) {
 Event* ptr;

        ptr = list.head;                        /* point to head node   */
        while ((ptr=ptr->getNext())  != NULL) {/* while next not NULL  */
            os << *ptr;
        }
        return os;
}

void E_List::operator>>(Event*& node) {
        
Event *first;        
//Event *ptr;                            /* This is modifyied part*/
                        
      if ((first = head->getNext()) == NULL) {
             node = NULL;
      } 
      else{
           head->setNext(first->getNext());
           first->setNext(NULL);
           node = first;
      }
           
}           
         
void* E_List::operator new(size_t size) {

 register E_List* ptr;

        if (freeList == NULL) {         /* is the free list empty?      */
            freeList = ptr              /* yes, allocate CHUNK of E_List*/
                     = (E_List*) new char[CHUNK * size];
            while (ptr != &freeList[CHUNK-1]) { /* while the last E_List*/
                                                /*  does not reach      */
                ptr->next = ptr + 1;    /* setup the next pointer       */
                ++ptr;                  /* skip to the next E_List      */
            }
            ptr->next = NULL;           /* set the next pointer of the  */
                                        /*  last E_List to NULL         */
        }
        ptr      = freeList;            /* get 1st E_List from free list*/
        freeList = freeList->next;      /* free list starts from the 2nd*/
         
        return(ptr);
}

void E_List::operator delete( void* l_ptr, size_t size) {
 Event* n_ptr;
 Event* first;
            
        first = ((E_List*) l_ptr)->head;/* first points the head node   */
        while ((n_ptr = first->getNext()) != NULL) {/* next not NULL?   */
            delete first;               /* delete the first node        */
            first = n_ptr;              /* first points to the next node*/
        }
        delete first;
         
        ((E_List*) l_ptr)->next = freeList;
        freeList = (E_List*) l_ptr;
}        

