#pragma once

#include "command.h"

#include <queue>

namespace stt { 
    enum class Command::Key;
}

/**
 * @class KeyQueue
 * FIFO - to act as a wrapper for commands to be queued.
 */
class KeyQueue {
public:
    void push(const stt::Command::Key& key);
    stt::Command::Key pop();
    bool is_empty() const;
private:
    /** @remark push(), pop(), is_empty() public methods forward Key(s) to 
    * underlying private std::queue. */
    std::queue<stt::Command::Key> _queue;
};
