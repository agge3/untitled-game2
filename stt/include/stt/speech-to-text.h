/** @file speech-to-text.h
 * @authors agge3, kpowkitty
 * SpeechToText private API, uses SFML and Mozilla Deepspeech.
 * @see command.h Command enumeration public interface (e.g., Command::Key).
 */

#pragma once

#include <deepspeech.h>
#include <SFML/Audio/SoundBufferRecorder.hpp>

#include <string>
#include <queue>

namespace stt {

/** @todo Sort KeyQueue class or just use std::queue. Weight benefits of 
 * wrapper and design of API. Design of API is a big TODO, but going for single
 * header, everything-included API right now.
 * @remark Uses native KeyQueue as a wrapper for internal key std::queue. */
//class KeyQueue;

/** @enum Key
*  Interface to obtain key pressed from SpeechToText.
*  @remark Size of enum is defined, to be forwarded.
*/
enum class Key {
    None = -1,
    A = 0,
    Up,
    Down,
    Left,
    Right,
    Play,
    Exit,
};

class SpeechToText {
public:
    SpeechToText();
    ~SpeechToText();

    /**
     * @struct AudioBuffer
     * SpeechToText AudioBuffer to store sf::SoundBuffer in a better API for 
     * DeepSpeech. 
     */
    struct AudioBuffer {
        unsigned int channels;
        const short* samples;
        unsigned int count;
    };

    /**
    * @enum Channels
    * Enum to have well-defined Channels.
    */
    enum class Channels : unsigned int {
        Mono = 1,
        Stereo = 2,
    };
    
    void run();
    std::string get_decoded();
    void clear();

    /** Get the next key. */
    Key get_key();
    /** Check if there are keys to get. */
    bool key_queue_is_empty() const;
    
    /** @todo Need? Get what type of key the current key is. */
    //static bool is_key_pressed(Key key);
private:
    void record();
    void decode();
    void parse();

    ModelState* _ctx;
    StreamingState* _sctx;
    const char* _mpath;
    const char* _spath;
    int _sample_rate;
    AudioBuffer _buf;
    std::string _decoded;
    sf::SoundBufferRecorder _recorder;
    std::queue<Key> _key_queue;
    
    /**
    * @var bool _run
    * Flag to confirm that SpeechToText should run (i.e., is not cleared).
    */
    bool _run;
};

}
