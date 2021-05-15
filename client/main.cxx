#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <regex>

#include "common/crossword.h"
#include "common/signals.h"
#include "common/defs.h"

using namespace std;

class Session{
    int sock;
    string name;
    Crossword crossword;

    void setup(){
        log("Setting up...");
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock == -1){
            perror("socket"); exit(1);
        }

        log("Connecting...");
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(ACC_PORT);
        if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1){
            perror("connect"); exit(1);
        }

        log("Connected to server!");
    }

    void log(string msg){
        cout << "Client>>> " << msg << endl;
    }

    void writeSock(signal_t &data, size_t len){
        if(write(sock, &data, len) == -1){
            perror("write"); exit(1);
        }
    }

    void readSock(signal_t &data){
        if(read(sock, &data, sizeof(data)) == -1){
            perror("read");
        }
    }

public:
    Session() : Session(""){}
    Session(string name) : name{name}, crossword{false}{
        setup();
        if(!name.empty()){
            setName(name);
        }
    }

    void setName(string nm){
        name = nm.length() > MAX_NAME_LENGTH ? nm.substr(0, MAX_NAME_LENGTH) : nm;
        signal_t sig{SET_NAME};
        sig.sigNo = SET_NAME;
        strcpy(sig.u.setName.name, name.c_str());
        writeSock(sig, sizeof(sig.sigNo)+sizeof(sig.u.setName));
    }

    void chat(string msg){
        signal_t sig{CHAT};
        strcpy(sig.u.chat.message, msg.c_str());
        writeSock(sig, sizeof(sig.sigNo) + msg.length() + 1);
    }

    void writeSquare(string cmd){
        regex pattern{"^(\\d+),(\\d+),([A-Z])$"};
        cmatch cm;
        if(!regex_match(cmd.c_str(), cm, pattern)){
            log("Invalid command");
            return;
        }

        signal_t sig{SET_SQUARE};
        sig.u.setSquare.x = stoi(cm[1].str());
        sig.u.setSquare.y = stoi(cm[2].str());
        sig.u.setSquare.letter = cm[3].str()[0];
        writeSock(sig, sizeof(sig.sigNo) + sizeof(sig.u.setSquare));
    }

    void show(){
        signal_t sig{GET_STATE};
        writeSock(sig, sizeof(sig.sigNo));

        readSock(sig);
        log("Got answer");
        Crossword cw{false};
        cw.create(sig.u.crosswordData.width, sig.u.crosswordData.height, sig.u.crosswordData.data);
        log("\n" + cw.to_string());
    }

    void leave(){
        close(sock);
        log("Goodbye!");
        exit(0);
    }
};
string execName;

void usage(){
    cout << "Usage: " << execName << " [userName]" << endl
    << "   userName                    - if provided, sets the user name directly" << endl << endl
    << "Command line arguments:" << endl
    << "   setName <name>              - set your name to <name>" << endl
    << "   show                        - show the crossword" << endl
    << "   chat <msg>                  - write a message in the chat" << endl
    << "   chat                        - show the chat" << endl
    << "   write <line>,<col>,<char>   - write the character <char> at line <line>, col <col> (zero inclusive)" << endl;
}

int main(int argc, char **argv){
    string name = "";
    if(argc >= 2){
        name = string{argv[1]};
    }

    execName = string{argv[0]};
    size_t pos;
    if((pos = execName.rfind('/')) != execName.npos){
        execName = execName.substr(pos+1);
    }

    Session session{name};

    char buf[512];
    for(;;){
        if(read(STDIN_FILENO, buf, sizeof(buf)) == -1){
            perror("read cmd"); exit(1);
        }
        string cmd{buf};
        auto nl = cmd.find('\n');
        cmd = cmd.substr(0, nl);
        if(!cmd.compare(0, 4, "exit") || !cmd.compare(0, 4, "quit")){
            session.leave();
        } else if(!cmd.compare(0, 7, "setName")){
            session.setName(cmd.substr(8));
        } else if(!cmd.compare(0, 4, "show")){
            session.show();
        } else if(!cmd.compare(0, 4, "chat")){
            session.chat(cmd.substr(5));
        } else if(!cmd.compare(0, 5, "write")) {
            session.writeSquare(cmd.substr(6));
        } else {
            usage();
        }
    }
}