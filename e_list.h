#ifndef         _E_LIST_H
#define         _E_LIST_H

#include        "event.h"

class E_List {
 private:
        Event*          head;           /* pointer to the head          */
                                        /*  node of the event list      */
        E_List*         next;            /* pointer to the next event    */
                                        /*  list (free list management) */   
        static E_List*  freeList;       /* pointer to the free list     */
 public:
        void    operator<<(Event&);     /* insert a event node  */
        friend ostream& operator<<( ostream&,   /* print out the list   */
                                    E_List&);
        void            operator>>(Event* &);    /* get first event node */
        int             operator!()             /* check list emptiness */
                        { return(head->getNext() == NULL); }
        void*           operator new(size_t);   /* new operator of      */
                                                /*  class E_List        */
        void            operator delete( void*, /* delete operator of   */
                                         size_t);/* class E_List        */
        E_List          ()                      /* constructor          */
                        { head = new Event; }
        ~E_List         ()                     /* destructor           */
                        { next = NULL; }
};

#endif  
