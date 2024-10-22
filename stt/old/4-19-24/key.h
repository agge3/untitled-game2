#pragma once

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
