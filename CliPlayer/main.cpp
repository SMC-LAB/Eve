#include "MarSystemManager.h"
using namespace Marsyas;
#include <iostream>
using namespace std;

void play(string fileName)
{
    MarSystemManager mng;
    MarSystem* net = mng.create("Series", "net");

    net->addMarSystem(mng.create("SoundFileSource", "src"));
    net->addMarSystem(mng.create("Gain", "gt"));
    net->addMarSystem(mng.create("AudioSink", "dest"));

    net->updControl("SoundFileSource/src/mrs_string/filename", fileName);
    net->updControl("SoundFileSource/src/mrs_natural/pos", 0);
    net->updControl("AudioSink/dest/mrs_bool/initAudio", true);

    while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->isTrue()) {
        net->tick();
    }

    delete net;
}

int main(int argc, char *argv[]) 
{
    string fileName;
    float gain, start;
    if (argc < 2)
    {
        cout << "Please enter filename." << endl;
        exit(1);
    }
    else
    {
        fileName = argv[1];
    }

    play(fileName);

    return 0;
}
