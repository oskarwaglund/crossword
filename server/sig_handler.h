#pragma once

#include "common/signals.h"
#include "common/crossword.h"
#include "client.h"


class SigHandler{
    void setName(setName_t &data, Client &client);
    void chat(chat_t &data, Client &client);
    void getState(Client &client, Crossword &crossword);
    void setSquare(setSquare_t &data, Client &client, Crossword &crossword);
    void writeSock(signal_t &data, size_t len, Client &client);
public:
    SigHandler();
    ~SigHandler();

    //Handle an incoming signal from a client
    void handle(signal_t signal, Client &client, Crossword &crossword);

    void log(string msg);
};