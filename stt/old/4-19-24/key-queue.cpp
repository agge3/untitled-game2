#include "key-queue.h"

void KeyQueue::push(const stt::Command::Key& key)
{
    _queue.push(key);
}

stt::Command::Key KeyQueue::pop()
{
    stt::Command::Key key = _queue.front();
    _queue.pop();
    return key;
}

bool KeyQueue::is_empty() const 
{
    return _queue.empty();
}
