#pragma once

#include <string>

using namespace std;

using byte = unsigned char;

class Crossword{
    byte width;
    byte height;
    int len;
    bool created;

    //A string that is width*height long
    string state;

    int posToInd(int x, int y);
    void indToPos(int ind, int *x, int *y);

    bool validState(string state);

public:
    // Set true to generate a default crossword
    Crossword(bool def);

    ~Crossword();


    bool create(int width, int height, string state);

    //Updates the crossword to the new state
    bool update(string state);

    bool setSquare(byte x, byte y, char letter);

    byte getWidth();
    byte getHeight();
    string getState();
    string to_string();
};