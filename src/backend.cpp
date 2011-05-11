#include "backend.h"
#include "util.h"

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

vector<soundFile>
SimplePlayerBackend::getSoundFileInfo()
{
    MarSystemManager mng;
    MarSystem* src = mng.create("SoundFileSource", "src");

    mrs_string filename = playbacknet->getctrl("SoundFileSource/src/mrs_string/filename")->to<mrs_string>();
    src->updControl("mrs_string/filename", filename);
    
    mrs_natural nLabels   = src->getControl("mrs_natural/nLabels")->to<mrs_natural>();
    mrs_natural numFiles  = src->getctrl("mrs_natural/numFiles")->to<mrs_natural>();
    mrs_string labelNames = src->getControl("mrs_string/labelNames")->to<mrs_string>();
    
    vector<string> labels = Util::split(labelNames, ',');
    
    vector<soundFile> soundFileDetails;

    for (int i = 0; i < numFiles; ++i)
    {
        mrs_string file       = src->getctrl("mrs_string/currentlyPlaying")->to<mrs_string>();
        mrs_natural ilabel    = src->getctrl("mrs_natural/currentLabel")->to<mrs_natural>();
        mrs_natural size      = src->getctrl("mrs_natural/size")->to<mrs_natural>();
        mrs_natural nchannels = src->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
        mrs_real srate        = src->getctrl("mrs_real/osrate")->to<mrs_real>();

        soundFile details = {file, labels[ilabel], size, nchannels, srate};
        soundFileDetails.push_back(details);

        src->updControl("mrs_natural/advance", i + 1);
    }

    return soundFileDetails;
}
