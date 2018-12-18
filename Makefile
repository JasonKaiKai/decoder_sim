CC = g++
CFLAGS = -g -c
LIBS = -lm
INCLUDE = -I/usr/include

all: main 

main:	random.o e_list.o event.o random.o simulation.o
	$(CC) -o $@ $? $(LIBS) 	

simulation.o:	simulation.cpp event.h e_list.h random.h mydef.h
	$(CC) $(CFLAGS) -o $@ simulation.cpp $(INCLUDE) 
 
random.o:	random.cc random.h
	$(CC) $(CFLAGS) -o $@ random.cc $(INCLUDE)
                                    
e_list.o:	e_list.cc e_list.h event.h
	$(CC) $(CFLAGS) -o $@ e_list.cc $(INCLUDE)

event.o:	event.cc event.h mydef.h
	$(CC) $(CFLAGS) -o $@ event.cc $(INCLUDE)  

clean:
	rm *.o 

