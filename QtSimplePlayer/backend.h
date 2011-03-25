#ifndef BACKEND_H
#define BACKEND_H

#include "MarSystemManager.h"
#include <iostream>
using namespace Marsyas;
using namespace std;

class SimplePlayerBackend 
{
public:
    SimplePlayerBackend();
    ~SimplePlayerBackend();
    
    MarSystem* getPlaybacknet();

    void open(string fileName);
    void close();
    void play();
    void pause();

private:
    MarSystem* playbacknet;
};

#endif //BACKEND_H
