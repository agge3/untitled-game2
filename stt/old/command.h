#pragma once

#include <queue>

namespace stt {

class Input {
public:
    /** @enum Key
    *  Interface to obtain key pressed from SpeechToText.
    */
    enum Key {
        Unknown = -1,
        A = 0,
        Up,
        Down,
        Left,
        Right,
    };
    enum class Scancode {
        Unknown = -1,
        A = 0,
        Up,
        Down,
        Left,
        Right,
    };
    static bool is_key_pressed(Key key);
    static bool is_key_pressed(Scancode code);
private:
    SpeechToText _stt;
    std::queue _keyqueue;
};

}
