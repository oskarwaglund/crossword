#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "sig_handler.h"

using namespace std;

SigHandler::SigHandler(){};
SigHandler::~SigHandler(){};

void SigHandler::writeSock(signal_t &data, size_t len, Client &client)
{
    if (write(client.getSocket(), &data, len) == -1)
    {
        perror("write");
        exit(1);
    }
}

void SigHandler::handle(signal_t signal, Client &client, Crossword &crossword)
{
    switch (signal.sigNo)
    {
    case SET_NAME:
        setName(signal.u.setName, client);
        break;
    case SET_SQUARE:
        setSquare(signal.u.setSquare, client, crossword);
        break;
    case CHAT:
        chat(signal.u.chat, client);
        break;
    case GET_STATE:
        getState(client, crossword);
        break;
    default:
        log("Got unknown signal " + to_string(signal.sigNo));
        break;
    }
}

void SigHandler::log(string msg)
{
    cout << "Server>>> " << msg << endl;
}

void SigHandler::setName(setName_t &data, Client &client)
{
    string previous = client.getName();
    data.name[sizeof(data.name) - 1] = 0;
    string name(data.name);
    client.setName(name);
    log(previous + " changed name to " + name + "!");
}

void SigHandler::chat(chat_t &data, Client &client)
{
    data.message[sizeof(data.message) - 1] = 0;
    string msg{data.message};
    log("Chat [" + client.getName() + "]: " + msg);
}

void SigHandler::setSquare(setSquare_t &data, Client &client, Crossword &crossword){
    if(crossword.setSquare(data.x, data.y, data.letter)){
        log(client.getName() + " wrote " + string{data.letter} + " at (" + to_string(data.x) + ", " + to_string(data.y) + ")");
    }
}

void SigHandler::getState(Client &client, Crossword &crossword)
{
    signal_t sig{UPDATE_STATE};
    sig.u.crosswordData.width = static_cast<byte>(crossword.getWidth());
    sig.u.crosswordData.height = static_cast<byte>(crossword.getHeight());
    strcpy(sig.u.crosswordData.data, crossword.getState().c_str());
    writeSock(sig, sizeof(sig.sigNo) + 2 + crossword.getState().length(), client);
}