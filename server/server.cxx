
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "client.h"
#include "common/defs.h"
#include "common/crossword.h"
#include "sig_handler.h"
using namespace std;

class Server{
    const int MAX_EVENTS = 8;
    const int RECV_BUF_SIZE = 1024;

    int accSocket;
    int epollFd;

    //Currently connected clients
    vector<Client> clients;

    int userCounter;

    Crossword crossword;
    SigHandler sigHandler;

public:
    Server() : crossword{true}, userCounter{0} {
        setup();
    }

    void setup(){
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(ACC_PORT);

        accSocket = socket(AF_INET, SOCK_STREAM, 0);

        if(accSocket < 0) {
            perror("socket"); exit(1);
        }

        if(bind(accSocket, (struct sockaddr *) &addr, sizeof(addr)) < 0){
            perror("bind"); exit(1);
        }

        if(listen(accSocket, 512) < 0){
            perror("listen"); exit(1);
        }

        epollFd = epoll_create(1);
        if(epollFd == -1){
            perror("epoll_create"); exit(1);
        }

        struct epoll_event ev;
        ev.data.fd = accSocket;
        ev.events = EPOLLIN;
        if(epoll_ctl(epollFd, EPOLL_CTL_ADD, accSocket, &ev) == -1){
            perror("epoll_ctl"); exit(1);
        }
        log("Server is set up!");
    }

    void run(){
        struct epoll_event events[MAX_EVENTS];
        int nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if(nfds < 0){
            perror("epoll_wait"); exit(1);
        }
        for(int i = 0; i < nfds; i++){
            if(events[i].data.fd == accSocket){
                newClient();
            } else if(auto client = matchSocketToClient(events[i].data.fd)){
                clientInput(*client);
            } else {
                log("Got unknown event");
            }
        }
    }

    void newClient(){
        log("A new client is connecting...");

        sockaddr addr;
        socklen_t len;
        int sock = accept(accSocket, &addr, &len);
        if(sock < 0){
            perror("accept"); exit(1);
        }

        string tmpName = "User_" + to_string(userCounter++);
        Client client(sock, tmpName);
        clients.push_back(client);

        epoll_event ev;
        ev.data.fd = sock;
        ev.events = EPOLLIN | EPOLLET;
        if(epoll_ctl(epollFd, EPOLL_CTL_ADD, sock, &ev) == -1){
            perror("epoll_ctl add client"); exit(-1);
        }

        log("Client " + tmpName + " connected!");
    }

    void clientInput(Client &client){
        char buf[RECV_BUF_SIZE];
        ssize_t rd = read(client.getSocket(), buf, sizeof(buf));
        if(rd < 0){
            perror("read from client"); exit(1);
        }
        signal_t *sig = reinterpret_cast<signal_t*>(buf);
        if(!sig){
            log("ERROR: Could not cast data");
            exit(1);
        }
        sigHandler.handle(*sig, client, crossword);
    }

    Client *matchSocketToClient(int socket){
        for(int i = 0; i < clients.size(); i++){
            if(socket == clients[i].getSocket()){
                return &clients[i];
            }
        }
        return nullptr;
    }

    void log(string msg){
        cout << "Server>>> " << msg << endl;
    }
};


int main(){
    Server server;
    for(;;){
        server.run();
    }
}
