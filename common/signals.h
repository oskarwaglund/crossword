#pragma once

#include "defs.h"

using byte = unsigned char;

typedef enum {
//From client
    SET_NAME,       //Set a name
    SET_SQUARE,     //Set a square
    GET_STATE,      //Request the current state
    CHAT,           //Post a message in the chat

//From server
    UPDATE_STATE,   //Tells a client that the state is updated
} sigNos_e;

//SET_NAME
typedef struct{
    char name[MAX_NAME_LENGTH+1];
} setName_t;

//SET_SQUARE
typedef struct{
    byte x;
    byte y;
    char letter;
} setSquare_t;

//GET_STATE (no data)

//CHAT
typedef struct{
    char message[512];
} chat_t;

//UPDATE_STATE
typedef struct{
    byte width;
    byte height;

    //data can be either
    //A-Z for letters
    //'#' for a black (non editable) square
    //' ' for an empty (editable) square
    char data[4096];
} crosswordData_t;


typedef struct signal_t {
    unsigned int sigNo; //sigNos_e
    union uni {
        setName_t setName;
        setSquare_t setSquare;
        chat_t chat;
        crosswordData_t crosswordData;
    } u;

    signal_t(unsigned no) : sigNo{no}{};
} signal_t;