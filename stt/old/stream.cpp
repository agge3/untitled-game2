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

#include <string>
#include <vector>
#include <iostream>
#include <typeinfo>
#include <sstream>
#include <ctdint>

enum class Channels : unsigned int {
    Mono = 1,
    Stereo = 2,
};



/** @note 16-bit, mono raw audio signal, SAMPLE RATE IS 16000. */

int main() {
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

    StreamingState* sctx = NULL;
    status = DS_CreateStream(ctx, &sctx);
    if (status != 0) {
        char* error = DS_ErrorCodeToErrorMessage(status);
        fprintf(stderr, "Could not create stream: %s\n", error);
        return 1;
    }
    else if (sctx == NULL) {
        fprintf(stderr, "Could not create stream: Streaming context is NULL\n");
        return 1;
    }

    /** @brief This should be addressed... */
    #if defined(__GNUC__) || defined(__GNUG__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wwrite-strings"
    #endif

    std::vector<char*> hotwords = {"up", "down", "left", "right"};
    for (int i = 0; i < hotwords.size(); ++i) {
        DS_AddHotWord(ctx, hotwords.at(0), 9.f);
    }
    std::vector<char*> coldwords = {"but", "at", "he", "ah", "the", "put", "bu"};
    for (int i = 0; i < coldwords.size(); ++i) {
        DS_AddHotWord(ctx, coldwords.at(0), -75.f);
    }

    #if defined(__GNUC__) || defined(__GNUG__)
        #pragma GCC diagnostic pop
    #endif

    // start the capture
    // NOTE: start(sampleRate = 44100), overwrite default to 16000 for DS
    std::cout << "Start recording\n";
    recorder.start(16000);

    // probably want some kind of set interval, not ideal, but workable
    // ...a better solution is overlapping and catting buffers, or having an
    // active buffer that is stream ... DS needs a buf though ... so not sure
    // (?)

    // xxx need to decide on correct buf interval
    sleep(1);

    // stop the capture
    recorder.stop();
    std::cout << "End recording\n";

    // retrieve the buffer that contains the captured audio data
    const sf::SoundBuffer& buffer = recorder.getBuffer();

    // need raw audio data
    // is vector<sf::Int16>
    // xxx convert sf::Int16 to short and std::vector to array - OK?
    const short* samples = static_cast<const short*>(buffer.getSamples());

    // is std::size_t
    unsigned int count = static_cast<unsigned int>(buffer.getSampleCount());

    // feed audio stream to DS and intermediate decode as streamed
    // opaque ptr, if NULL error has occured -> pass to DS_CreateStream()

    std::cout << "Start decoding\n";
    char* stt = DS_SpeechToText(ctx, samples, count);
    std::cout << "End decoding\n";

    std::cout << stt << "\n";

    // convert stt to cpp string
    std::string stt_cpp = stt;
    // now free stt from memory, cpp string will handle memory management
    DS_FreeString(stt);

    /** @todo Cheap check before expensive parsing? */
    //std::string keys = "up down left right";
    //// check before parsing, will save unnecessary cpu time
    //std::size_t found = stt_cpp.find(keys);
    //if (found == std::string::npos) {
    //    std::cout << "No keys found this buffer!\n";
    //}

    /** @brief Parse CXX string for key inputs. */
    std::istringstream in(stt_cpp);
    std::string parse;
    int up, down, left, right;
    up = down = left = right = 0;
    while (in >> parse) {
        if (parse == "up") ++up;
        else if (parse == "down") ++down;
        else if (parse == "left") ++left;
        else if (parse == "right") ++right;
        else continue;
    }
    std::uint16_t bitstring = 0000000000000000;
    // 2^0, 2^1, 2^2, 2^3, 2^4, 2^5, 2^6, 2^7, 2^8, ... 2^15
    for(int i = 0; i < up; ++i) {
        bitstring += 2^i;
    }
    for(int i = 0; i < down; ++i) {
        // starting at 3 idx
        bitstring += 2^(i+3);
    }
    for(int i = 0; i < left; ++i) {
        // starting at 7 idx
        bitstring += 2^(i+7);
    }
    for(int i = 0; i < right; ++i) {
        // starting at 11 idx
        bitstring += 2^(i+11);
    }

    // print key presses to confirm parser is working correctly
    std::cout << "Up: " << up << "\nDown: " << down << "\nLeft:" << left
        << "\nRight: " << right << "\n";

    // end decoding EARLY, have already decoded on-the-fly, avoid costly final
    // decode
    DS_FreeStream(sctx);

    // ctx (local), short int
    //DS_SpeechToText(ctx, abuf, asize);
    // need to DS_FreeString from mem
    // is going to ret char*

    DS_FreeModel(ctx);
    return 0;
}

stt::Input::Key to_key(uint8_t bitstring, int count) {
    // binary-and the bit string to get the keys
    std::uint16_t up = 0000000000001111;
    std::uint16_t down = 0000000011110000;
    std::uint16_t left = 000011110000000;
    std::uint16_t right = 1111000000000000;
    switch (bitstring) {
    case up | bitstring < 2^3 && up | bitstring > 0:
        switch (bitstring) {
        case 2^0:
            count = 1;
        case 2^1:
            count = 2;
        case 2^2
            count = 3;
        }
        return stt::Input::Up
        break;
    case down | bitstring < 2^7 && down | bitstring > 2^3:
        return stt::Input::Down;
        break;
    case left | bitstring < 2^11 && left | bitstring > 2^7:
        return stt::Input::Left;
        break;
    case right | bitstring < 2^15 && right | bitstring > 2^15:
        return stt::Input::Right;
        break;
    default:
        break;
    }

    unsigned int up_presses = static_cast<unsigned int>(bitstring | up);
    unsigned int down_presses = static_cast<unsigned int>(bitstring | down);
    unsigned int left_presses = static_cast<unsigned int>(bitstring | left);
    unsigned int right_presses = static_cast<unsigned int>(bitstring | right);
}
