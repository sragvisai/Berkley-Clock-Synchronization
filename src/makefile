 #defining the compiler
	GG = g++
	CFLAGS = -g -Wall -c
 #compile command  
  compile : daemon processes
  
  daemon: daemon.o
	$(GG) daemon.o -o daemon -lpthread ../other/myqu.cpp
  processes: processes.o
	$(GG) processes.o -o processes -pthread

 daemon.o: daemon.cpp
	$(GG) -c -g daemon.cpp ../other/myqu.cpp -pthread
 processes.o:processes.cpp
	$(GG) -std=c++1z -c processes.cpp
	
  clean:
	rm -rf *.o compile
