#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <string>
#include <bits/stdc++.h>

#define PORT 8080

using namespace std ;

struct clinetArgs {
    int socketNumber ;
};

void * msgHandler(void * clientArg);
vector<string> splitMsg(string str);
bool checkFileExist(const string& fileName);

int main(int argc, char const* argv[]){

    int server_fd ,new_socket, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    server_fd =  socket(AF_INET, SOCK_STREAM, 0) ;
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    //address.sin_port = htons(atoi(argv[1]));
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,sizeof(address))< 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }


    while (1){

        cout << "Waiting for client msgs" << endl;
        cout << flush;

        new_socket = accept(server_fd, (struct sockaddr*)&address,(socklen_t*)&addrlen) ;
        if (new_socket< 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pthread_t newThread ;
        clinetArgs  clientArg;
        clientArg.socketNumber = new_socket ;

        pthread_create(&newThread, NULL, msgHandler,&clientArg);


    }

    return 0;
}


void * msgHandler(void * clientArg){
    clinetArgs * args = (clinetArgs *)clientArg;
    int newSocket = args->socketNumber ;


    string fullMsg = "" ;
    //start reading the msg from the client
    while (1){
        char buffer[4096] = {0};
        long long valread = read(newSocket,buffer,4096);

        //if its empty or there is no more to read then break
        if (valread <= 0)
            break;
        cout << buffer << endl ;
        cout << flush ;

        buffer[strlen(buffer) - 1] = '\0';
        fullMsg += buffer ;
    }

    vector<string> splits = splitMsg(fullMsg);
    if (splits[0] == "get"){
        string fileName = splits[1];
        if (checkFileExist(fileName)) {
            char *OK_RESPOND = "HTTP/1.1 200 OK GET\\r\\n \n";
            send(newSocket, OK_RESPOND, strlen(OK_RESPOND), 0);
            
            //send data of that file 
            // char * data ; 
            // send(newSocket, data, strlen(data), 0);
        }
        else{
            char *notfound = "HTTP/1.1 404 NOT FOUND\\r\\n \n";
            send(newSocket, notfound, strlen(notfound), 0);
        }
    }
    else if(splits[0] == "post"){
        string fileName = splits[1];
        if (checkFileExist(fileName)) {
            char *OK_RESPOND = "HTTP/1.1 200 OK POST\\r\\n \n";
            send(newSocket, OK_RESPOND, strlen(OK_RESPOND), 0);
        }
        else{
            char *notfound = "HTTP/1.1 200 NOT FOUND\\r\\n \n";
            send(newSocket, notfound, strlen(notfound), 0);
        }
    }
    else if(splits[0] == "close"){
        return NULL ;
    }

    close(newSocket);
    return NULL;
}

vector<string> splitMsg(string str){
    vector<string> splits;
    string token  = "";
    for (char c : str){
        if (c != ' ') {
            token += c;
        }
        //if we found space but the previous char was also space then continue
        else if (c == ' ' && !token.empty()){
            splits.push_back(token);
            token.clear();
        }
    }
    return splits;
}

bool checkFileExist(const string& fileName){
    ifstream file;
    file.open(fileName);
    if(file)
        return true;
    else
        return false;
}
