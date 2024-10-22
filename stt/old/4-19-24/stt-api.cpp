#include "speech-to-text.h"
#include "key-queue.h"
#include "stt-api.h"

#include <sstream>
#include <memory>

namespace {
    using Ptr = std::unique_ptr<SpeechToText, void(*) (SpeechToText*)>;
    void delete_ptr(SpeechToText* stt) { delete stt; }
    void do_not_delete_ptr(SpeechToText* stt) {}
}
    
bool stt::Command::is_key_pressed(stt::Command::Key key) {
    // xxx why doesn't it work with !is_empty() (?)
    if (_key_queue->is_empty())
        return false;

    return true;
}

stt::Command::Key stt::Command::get_key() {
    return _key_queue->pop();
}

bool stt::Command::is_stt_key(stt::Command::Key key) {
    if (_key_queue->is_empty())
        return false;

    return true;
}

bool stt::

void stt::init()
{
    Ptr stt(new SpeechToText());
    do_not_delete_ptr(stt);
}

void run()
{

void exit()
{
    
/** Get the next key. */
Key get_key()

// xxx should these be static (?)
/** Check if there are keys to get. */
static bool get_keys();
/** Get what type of key the current key is. */
static bool get_key_type(Key key);
