#ifndef HEX_H
#define HEX_H

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

// this is the stupidest shit i've ever written bc i was atcually writing it then it inexplicably wouldn't work
// then i did this, it worked and my jaw dropped

string ptrToHex(void* x){
    stringstream ss;
    ss << x;
    return ss.str();
}

void* hexToPtr(string &s){
    void* out;
    stringstream ss;
    ss << s;
    ss >> out;
    return out;
}

#endif