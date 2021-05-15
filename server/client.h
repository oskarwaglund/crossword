#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <string>

using namespace std;

class Client
{
    //TCP socket to client
    int socket;

    //Name of the client
    string name;

public:
    Client(int socket, string name);
    ~Client();

    string getName()
    {
        return name;
    }

    int getSocket()
    {
        return socket;
    }

    void setName(string nm)
    {
        name = nm;
    }
};