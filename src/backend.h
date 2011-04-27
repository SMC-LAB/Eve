#ifndef BACKEND_H
#define BACKEND_H

#include "MarSystemManager.h"
#include <iostream>
using namespace Marsyas;
using namespace std;

typedef struct
{
    mrs_string file;
    mrs_string label;
    mrs_natural size;
    mrs_natural nchannels;
    mrs_real srate;
} soundFile;

static vector<string> split(const string& s, char c);

class SimplePlayerBackend 
{
public:
    SimplePlayerBackend();
    ~SimplePlayerBackend();
    MarSystem* getPlaybacknet();
    vector<soundFile> getSoundFileInfo();

private:
    MarSystem* playbacknet;
};


#endif //BACKEND_H
