#include "backend.h"

SimplePlayerBackend::SimplePlayerBackend()
{
    MarSystemManager mng;
    playbacknet = mng.create("Series", "playbacknet");
    playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
    playbacknet->addMarSystem(mng.create("Gain", "gain"));
    playbacknet->addMarSystem(mng.create("AudioSink", "dest"));
    playbacknet->updControl("AudioSink/dest/mrs_bool/initAudio", true);
}

SimplePlayerBackend::~SimplePlayerBackend()
{
    delete playbacknet;
}

MarSystem* SimplePlayerBackend::getPlaybacknet()
{
    return playbacknet;
}

void SimplePlayerBackend::open(string fileName) 
{
    cout << "SimplePlayerBackend: Opening " << fileName << endl;

    //close();
    //playbacknet->updControl("AudioSink/dest/mrs_bool/initAudio", true);
    playbacknet->updControl("SoundFileSource/src/mrs_natural/pos", 0);
    playbacknet->updControl("SoundFileSource/src/mrs_string/filename", fileName);
}

void SimplePlayerBackend::close()
{
    cout << "SimplePlayerBackend: Closing current file" << endl;

    playbacknet->updControl("SoundFileSource/src/mrs_string/fileName", "");
    playbacknet->updControl("AudioSink/dest/mrs_bool/initAudio", false);    
}
