#pragma once

namespace stt {
    /** @enum Key
    *  Interface to obtain key pressed from SpeechToText.
    *  @remark Size of enum is defined, to be forwarded.
    */
    enum class Key : int {
        Unknown = -1,
        A = 0,
        Up,
        Down,
        Left,
        Right,
    };

    void init();
    void run();
    void exit();
    /** Get the next key. */
    Key get_key()

    // xxx should these be static (?)
    /** Check if there are keys to get. */
    static bool get_keys();
    /** Get what type of key the current key is. */
    static bool get_key_type(Key key);
}
