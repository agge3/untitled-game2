/**
* @file recorder.h
* @authors agge3, kpowkitty
* Override SFML recorder class to have expected audio data for DeepSpeech.
*/

#pragma once

#include "SFML/Audio/SoundRecorder.hpp"

class MyRecorder : public sf::SoundRecorder {
public:
    //virtual bool onStart() // optional
    //{
    //    return true;
    //}

    virtual bool onProcessSamples(const sf::Int16* samples, std::size_t sampleCount)
    {
        // do something useful with the new chunk of samples
        ...

        // return true to continue the capture, or false to stop it
        return true;
    }

    virtual void onStop() // optional
    {
        // clean up whatever has to be done after the capture is finished
        ...
    }
private:
    static const short samples = 0; // need sample size (?)
    // don't forget: static_cast<sf::Int16>(samples);
    static const std::size_t sample_count = 0; // need sample count (?)
    static const unsigned_int channels = 1; // mono
};
