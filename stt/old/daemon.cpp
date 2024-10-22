/** @file daemon.cpp
* @authors agge3, kpowkitty
* Polls for sound and puts in sound stream.
*/

#include "recorder.h"

#include <SFML/Audio/SoundStream.hpp>
#include <SFML/Audio/SoundRecorder.hpp>

#include <string>
#include <vector>

SoundRecorder vin;

int init() {

    // list avail devices
    std::vector<std::string> available_devices = sf::SoundRecorder::getAvailableDevices();
    // choose first device ... better being scripted (?)
    std::string input_device = available_devices[0];

    // set recorder
    if (!vin.setDevice(input_device)) {
        std::cerr << "Sound device is not available\n";
        return 1; 
    }
        
    // if (!vin.isAvailable()) {} // (?)
    
    if (!vin.onStart()) {
        // ds not available
        std::cerr << "DeepSpeech is not available\n"
        return 1;
    }

    if (vin.onProcessSamples()) {

    const sf::Int16* samples = buffer.getSamples();
    std::size_t count = buffer.getSampleCount();
    // doSomething(samples, count);   
        

    return 0;
void run() {
    // stream doesn't care about sound source, need a sound source
    // open the stream and send to buffer
    // want a way to throw away garbage
    // and need a proper buffer size to parse words/forward to ds
    while(true) {
        // polls every second
        // prolly sf::clock for dt - prolly needs to be 60fps (60 polls per sec)
        // need maybe some async here... bc ds processing will hang sound recorder

        // all this is a daemon - it polls and maintains open stream
        // -> that is being forwarded to ds
    
    // 1. have sound recorder running, live buffer
            // TEST playback of buffer
            // gonna have some wasted code to implement buffer cap, but it's fine - VERY IMPORTANT

    // 2. be fwding live buffer to deepspeech - does it need be in daemon or can daemon call it (?)
            // compilation test before output to file
            // TEST decoding - prolly to file would be best
        // END OF DAEMON
        
    // 3. need file that exists
    //      deepspeech outputs stt to file
    //


    // sstringstream my_string;
    // my_string << char DS_SpeechToText

            // OPT: potential string stream that cin/lua just reads for patterns?
    

    // parse output of deepspeech
    //  - look for keywords

            // prolly its own daemon/while poll that has a switch statement and
            // returns an enumerated command

    
    // voice::Up
    //
    // switch(key)
    //  move up
    //  move down

    // switch(in)
    //  case "up":
    //      return usigned_int 1
    //
    // case "down":
    //      return usigned_int 1 << 1
    //
    //
    //  enum Key {
    //      Up = 1
    //
    //      somehow make an interface
    //
    //
    //      ... just an idea
    
    // somehow?? make an interface






    }  
}
