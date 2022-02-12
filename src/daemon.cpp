//g++ -g daemon.cpp myqu.cpp -o daemon -pthread
//Author : Sai Sragvi Vibhushan N
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdbool.h>
#include<limits.h>
#include<pthread.h>
#include<iostream>
#include "../other/mque.h"
#include"unistd.h"
#include<string.h>
#include<mutex>
#include<fstream>
#include <bits/stdc++.h>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <atomic>
#include <filesystem>
#include<random>
#include<time.h>


int avg_time = 0;
int total_count =  0;
int runningcount = 0;
int globaltime = 0;

using namespace std;

//to store the file path
string path;

#define SERVERPORT 8500
#define BUFSIZE 4096
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 10
#define THREAD_POOL 2000

//declaring the mutex locks for CS and the conditional variables
pthread_t thread_box[THREAD_POOL];
pthread_mutex_t mutex_thread = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

std::mutex got;

int updatedcount = 0 ;

typedef struct sockaddr_in  SA_IN;
typedef struct sockaddr SA;

void handle_connection(int  *p_client_socket, char* path);
int error_check(int exp, const char *msg);
void *thread_func(void *arg);


//load the data from file into the data records

void update(int a) {

//cout<<"Inside the update function--"<<a<<endl;

avg_time+=a;
runningcount++;
//cout<<"After the update"<<avg_time<<endl;
sleep(0.5);
}

int getcount(){
	return updatedcount;
}

void updatecount(){
	updatedcount++;
}

int gettotalcount() {

return total_count;
}

int getavgtime() {
return avg_time;
}

int getrunningcount(){
	return runningcount;
}

void updatetotalcount(){

	total_count++;
}

int getmytime(){

	return globaltime;
}

void updateglobaltime(int x){

	globaltime+=x;
}

int main(){
	
	int flag = 1; 
	
	int server_socket,client_socket,addr_size;
	SA_IN server_addr, client_addr;
	
	srand (time(0));
	globaltime = 1 + rand()%10;
	cout<<"My time right now is:-"<<globaltime<<endl;
	//create a thread pool
	for(int i = 0; i<THREAD_POOL;i++){
		pthread_create(&thread_box[i],NULL,thread_func,NULL);
	}

	error_check((server_socket = socket(AF_INET,SOCK_STREAM,0)),"Failed");

	//address struct
	server_addr.sin_family =  AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(SERVERPORT);

	error_check(bind(server_socket,(SA*)&server_addr,sizeof(server_addr)),"Failed at bind");

	error_check(listen(server_socket,SERVER_BACKLOG),"Failed at listen");

	while(flag == 1){
	
		cout<<"Waiting for processes to connect..."<<endl;
		addr_size = sizeof(SA_IN);
		error_check(client_socket = 
				accept(server_socket,(SA*)&client_addr,
					(socklen_t*)&addr_size),"accept failed");
		cout<<"Connected"<<endl;
		updatetotalcount();
		
		int *pclient = (int*)malloc(sizeof(int));
		*pclient = client_socket;
		
		//the following block related to thread pool is referred from Jacob Sorber's tutorial
		//multithreading
		
		//as soon as a connection is established push it into the queue
		//applied the locks so that dequeue and enqueue doesnt happen at the same time
		//and dont result in invalid pointers
		pthread_mutex_lock(&mutex_thread);
		enq(pclient);
		pthread_cond_signal(&condition_var);
		pthread_mutex_unlock(&mutex_thread);
		
		//cin>>flag;
			
	}
	
	
	
	
	return 0;
}

//a function to print error msgs if any
int error_check(int exp, const char *msg) {

	if(exp == SOCKETERROR){
		perror(msg);
		exit(1);
	}

	return exp; 
}





void *thread_func (void *arg) {
	char* temp_path = (char*) arg;
	while(true) {
	int *pclient;
	pthread_mutex_lock(&mutex_thread);
	if((pclient = dequ()) == NULL){
	
	//wait until a u get a thread enqueued and if any thread is available pull it off and deal with it
	pthread_cond_wait(&condition_var,&mutex_thread);
	
	pclient = dequ();
	}
	pthread_mutex_unlock(&mutex_thread);
	

	if(pclient!=NULL) {
	//we have a connection
	
		handle_connection(pclient,temp_path);
	}
	}
}

void handle_connection(int* p_client_socket, char* path) {
	
	//cout<<"Inside handle connection function "<<*p_client_socket<<endl;//<<"also this"<<data[0].balance;
	int client_socket = *p_client_socket;
	
	//lets free the pointer	
	free(p_client_socket);
	
	//variables for messages that are to be sent and received
	char buffer[BUFSIZE];
	bzero(buffer,BUFSIZE);
	size_t bytes_read;
	int msgsize = 0;
	
	int updated,randtime,client_time,difference;
	const char* temp;
	const char* jh;
	
	
	//generating a random number
	
	randtime = getmytime();
	
	//sending the time to clients first
	stringstream timestamp;
	timestamp << randtime ;
	temp = timestamp.str().c_str();
	send(client_socket,temp,strlen(temp),0);
	
	cout<<"Sent time:"<<*temp<<endl;
	
	//recieve the time difference
	
	if(recv(client_socket , buffer , BUFSIZE , 0) < 0 ) {
		cout<<"Message was not successfully received by the server"<<endl;
	}
	else{
	cout<<"----------------"<<endl;
	//cout<<"Time received from one of the processes - "<<buffer<<endl;
	}
	
	error_check(bytes_read,"rec error");
	buffer[msgsize -1] - 0;

	fflush(stdout);
	
	client_time = stoi(buffer);
	cout<<" One of the processes time is ---"<<client_time<<endl;
	got.lock();
	update(client_time);
	got.unlock();
	
	/*cout<<"Second time---";
	temp = strtok(NULL, " ");
	cout<<temp<<endl;*/
	
	cout<<"The total count is"<<gettotalcount()<<"Running count is "<<getrunningcount()<<endl;
	while(gettotalcount() != getrunningcount()){
	//cout<<"The values are" << "--totalcount--"<<gettotalcount()<<"--temp--"<<getrunningcount()<<endl;
	//cout<<"Sleeping for 1 sec"<<endl;
	sleep(1);
	}
	
	//calculating the difference
	stringstream ss;
	
	//cout<<"GetAVGtime--"<<getavgtime()<<"---totalcount--"<<gettotalcount()<<endl;
	difference = getavgtime()/(gettotalcount()+1) - client_time;
	cout<<"the difference is:"<<difference<<endl;
	ss << difference;
	jh = ss.str().c_str();
	
	//updategolbaltime(0 - getavgtime()/(gettotalcount()+1));
	
	//sending difference to be upated  to the client based
	send(client_socket,jh,strlen(jh),0);
	
	//closing the socket
	close(client_socket);
	
	//filling out the buffer with zeroes
	//bzero(msg,strlen(msg));
	
	
	//updating daemons clock
	
	if(getcount()<1){
	updatecount();
	updateglobaltime((getavgtime()/(gettotalcount()+1))-0);
	
	int finaltime = getmytime();
	
	cout<<"Final time of the daemon is"<<finaltime<<endl;
	}
	cout<<"closing connection"<<endl;
	
				
	cout<<"Press ctrl+c to exit from the server if you are done with it"<<endl;
	cout<<"---------------------"<<endl;
	
	
}
