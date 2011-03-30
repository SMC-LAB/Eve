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
