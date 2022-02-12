//g++ -std=c++1z processes.cpp
//Author : Sai Sragvi Vibhushan N
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <atomic>
#include <filesystem>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include<random>
#include<time.h>

using std::cout; using std::endl;
using std::string; using std::filesystem::exists;

//constexpr std::atomic<int> FORK_NUM = 3;
int FORK_NUM = 0 ;
constexpr std::atomic<int> handler_exit_code = 103;

std::atomic<int> child_pid;
std::atomic<int> *children;

using namespace std;

void sigquitHandler(int signal_number)
{
    for (int i = 0; i < FORK_NUM; ++i) {
        kill(children[i], SIGTERM);
    }
    while ((child_pid = wait(nullptr)) > 0);
    _exit(handler_exit_code);
}

pid_t spawnChild()
{
    pid_t ch_pid = fork();
    //cout<<"Im here"<<endl;
    if (ch_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (ch_pid > 0) {
        cout << "spawn child with pid - " << ch_pid << endl;
        return ch_pid;
    } else {
    	
  //cout<<"Out here with id as " <<getpid()<<endl;  
  char buffer[4096];
  int clientSocket;
  char msg[1000];
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  //creating a temporarty var to get the time from the line of req
  
            //char temp[100];
  

	    // Create the socket. 
	    clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	  //Configure settings of the server address
	 // Address family is Internet 
	  serverAddr.sin_family = AF_INET;

	  //Set port number, using htons function 
	  serverAddr.sin_port = htons(8500);

	 //Set IP address to localhost
	  serverAddr.sin_addr.s_addr = INADDR_ANY;
	  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	    //Connect the socket to the server using the address
	    addr_size = sizeof serverAddr;
	    if(connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size)!=-1)
	    cout<<"Client successfully connected"<<endl;
	
	int randtime=0,dtime=0,difference=0;
	const char* temp= NULL;
	
	randtime = 1+ getpid()%10;
	
	cout<<"My time is"<<randtime<<endl<<endl;
	
	bzero(buffer,4096);
	
	//first receive to get the time daemons value
	if(recv(clientSocket,buffer,1024,0)<0) {
	
		cout<<"Received failed"<<endl;
	}
	else{
	cout<<"Daemon's time is---"<<buffer<<endl;
	
	}
	
	//daemon time
	
	dtime = stoi(buffer);	
	difference = randtime - dtime;
	
	stringstream timestamp ;
	timestamp << difference;
	temp = timestamp.str().c_str();
	//cout<<"Temp is---"<<temp<<"----"<<difference<<endl;
	
	strcpy(msg,temp);
				
	if( send(clientSocket,msg,strlen(msg),0)<0) {
		cout<<"Send failed"<<endl;
	}
	else{
		cout<<"Sending my diff in time to the daemon-"<<msg<<endl;
	}
	
	
	bzero(buffer,4096);
	
	if(recv(clientSocket,buffer,1024,0)<0) {
	
		cout<<"Received failed"<<endl;
	}
	else{
	cout<<"For process with time clock difference :"<<randtime<<"/Difference sent from the server is-:"<<buffer<<endl<<endl;
	randtime += stoi(buffer);
	cout<<"My old time was-: "<<randtime-stoi(buffer)<<"My new time is-: "<<randtime<<endl;
	cout<<"--------------------------"<<endl;
	}
 
    close(clientSocket);
}
        exit(EXIT_FAILURE);
}

int main() {
	
	cout<<"Enter the # of processes that you want to be part of this networkd apart of daemon"<<endl;
	cin>>FORK_NUM;
    children = reinterpret_cast<std::atomic<int> *>(new int[FORK_NUM]);
    signal(SIGQUIT, sigquitHandler);

    srand (time(NULL));
    
    children = reinterpret_cast<std::atomic<int> *>(new int[FORK_NUM]);
    
    for (int i = 0; i < FORK_NUM; ++i) {
        children[i] = spawnChild();
    }
    
    cout << endl;
    cout<<endl;
    while ((child_pid = wait(nullptr)) > 0)
        cout << "child " << child_pid << " terminated" << endl;

    return EXIT_SUCCESS;
}

