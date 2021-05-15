#include <stdlib.h>
#include <string.h>
#include <sstream>

#include "crossword.h"

const int defaultW = 5;
const int defaultH = 5;
const string defaultStr = "     ##     ##  #       #";

Crossword::Crossword(bool def){
    created = false;
    if(def){
        create(defaultW, defaultH, defaultStr);
    }
}

Crossword::~Crossword(){}

bool Crossword::create(int w, int h, string st){
    width = w;
    height = h;
    len = w*h;

    if(created){
        return false;
    }

    state = st;
    created = true;

    return true;
}

bool Crossword::validState(string state){
    if(state.length() != len){
        return false;
    }
    for(int i = 0; i < len; i++){
        byte b = state.at(i);
        if(b != '#' && b != ' ' && b < 'A' && b > 'Z'){
            return false;
        }
    }
    return true;
}

bool Crossword::update(string state){
    if(!created || !validState(state)){
        return false;
    }
}

bool Crossword::setSquare(byte x, byte y, char letter){
    int ind = posToInd(x, y);
    if(ind >= len){
        return false;
    }
    state.replace(ind, 1, string{letter});
}

byte Crossword::getWidth(){
    return width;
}
byte Crossword::getHeight(){
    return height;
}
string Crossword::getState(){
    return state;
}
int Crossword::posToInd(int x, int y){
    return y*width + x;
}
void Crossword::indToPos(int ind, int *x, int *y){
    *x = ind % width;
    *y = ind/width;
}
string Crossword::to_string(){
    stringstream res;
    res << "+" << string(width, '-') << "+\n";
    for(int i = 0; i < len; i+= width){
        res << "|" << state.substr(i, width) << "|" << '\n';
    }
    res << "+" << string(width, '-') << "+\n";
    return res.str();
}