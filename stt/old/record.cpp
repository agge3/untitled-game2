/* Project: occ-accessibility-tour
 * Sub-project: voice
 * Developers: @agge3, @kpowkitty
 * Date: February 2024
 */

#include <deepspeech.h>
#include <SFML/Audio/SoundBufferRecorder.hpp>

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <typeinfo>

enum class Channels : unsigned int {
    Mono = 1,
    Stereo = 2,
};

int main() {

    // first check if an input audio device is available on the system
    if (!sf::SoundBufferRecorder::isAvailable()) {
        std::cerr << "Audio capture is not available on this system.\n";
    }
    
    // create the recorder
    sf::SoundBufferRecorder recorder;

    // xxx static_cast? what's the point of the enum, sort out better
    recorder.setChannelCount(static_cast<unsigned int>(Channels::Mono));
    std::cout << typeid(Channels::Mono).name() << "\n"
        << static_cast<unsigned int>(Channels::Mono) << "\n";
    
    // start the capture
    // NOTE: start(sampleRate = 44100), overwrite default to 16000 for DS
    recorder.start(16000);
    
    // probably want some kind of set interval, not ideal, but workable
    // ...a better solution is overlapping and catting buffers, or having an 
    // active buffer that is stream ... DS needs a buf though ... so not sure 
    // (?)
   
    std::cout << "Sleeping...\n";
    sleep(10);
    std::cout << "Sound recorded\n";    

    // stop the capture
    recorder.stop();
    
    // retrieve the buffer that contains the captured audio data
    const sf::SoundBuffer& buffer = recorder.getBuffer();

    // tmp var for status output
    int tmp = 0;
    std::cout << typeid(tmp).name() << "\n";

    // need raw audio data
    // is vector<sf::Int16>
    // xxx convert sf::Int16 to short and std::vector to array - OK?
    const short* samples = static_cast<const short*>(buffer.getSamples());
    std::cout << typeid(samples).name() << "\n"
        << "\'Samples\' address:" << samples << "\n"
        << "\'Samples\' data:" << samples[0] << "\n";

    // is std::size_t
    unsigned int count = static_cast<unsigned int>(buffer.getSampleCount());
    std::cout << typeid(count).name() << "\n"
        << count << "\n";
    
    // init ds 
    ModelState* ctx;

    // model & scorer path
    static const char mpath[] = 
        "../dep/deepspeech-models/deepspeech-0.9.3-models.pbmm";
    static const char spath[] = 
        "../dep/deepspeech-models/deepspeech-0.9.3-models.scorer";
    
    int status = DS_CreateModel(mpath, &ctx);
    if (status != 0) {
        char* error = DS_ErrorCodeToErrorMessage(status);
        fprintf(stderr, "Could not create model: %s\n", error);
        free(error);
        return 1;
    }

    // model's created
    
    // TODO: beamwidth? 

    // now scorer

    status = DS_EnableExternalScorer(ctx, spath);
    if (status != 0) {
        fprintf(stderr, "Could not enable external scorer.\n");
        return 1;
    // TODO: alpha/beta?
    }

    // util: get expected model sample rate
    //if (DS_GetModelSampleRate(ctx) != sample rate we have...) {
    
    tmp = DS_GetModelSampleRate(ctx);
    std::cout << tmp << "\n";

    // NOTE: 16-bit, mono raw audio signal, SAMPLE RATE IS 16000
    // NOTE: sf::SoundStream?
    // NOTE: DS_FeedAudioContent(
    

    // STT
    char* stt = DS_SpeechToText(ctx, samples, count);
    std::cout << stt << std::endl;
    
    // free stt from mem
    DS_FreeString(stt);

    // ctx (local), short int 
    //DS_SpeechToText(ctx, abuf, asize);
    // need to DS_FreeString from mem
    // is going to ret char*
    
    DS_FreeModel(ctx);
    return 0;
}
