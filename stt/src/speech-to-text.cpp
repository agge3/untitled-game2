/* Project: occ-accessibility-tour
 * Sub-project: voice
 * Developers: @agge3, @kpowkitty
 * Date: February 2024
 */

#include "speech-to-text.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <errno.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <vector>
#include <iostream>
#include <cassert>
#include <sstream>

static const char MODEL_PATH[] =
    "../dep/deepspeech-models/deepspeech-0.9.3-models.pbmm";
static const char SCORER_PATH[] =
    //"../dep/deepspeech-models/deepspeech-0.9.3-models.scorer";
    "../dep/deepspeech-models/kenlm-no-a.scorer";

stt::SpeechToText::SpeechToText() :
    _mpath(MODEL_PATH),
    _spath(SCORER_PATH),
    _ctx(),
    _sctx(),
    _sample_rate(16000),
    _buf(),
    _decoded(), // xxx std::optional to handle empty string instantiation (?)
    _recorder(),
    _key_queue(),
    _run()
{
    /** @brief Instantiate AudioBuffer.
     * @note Only channels are known (Channels::Mono), sf::SoundBuffer will
     * instantiate samples and count. */
    _buf.channels = static_cast<unsigned int>(Channels::Mono);

    /** @brief Create DeepSpeech model with model path and model context. */
    int status = DS_CreateModel(_mpath, &_ctx);
    if (status != 0) {
        char* error = DS_ErrorCodeToErrorMessage(status);
        fprintf(stderr, "Could not create model: %s\n", error);
        free(error);
        // xxx need to try-catch block (?), might not work - or init in init()
        //return 1;
    }

    /** @brief Add DeepSpeech scorer to model context. */
    status = DS_EnableExternalScorer(_ctx, _spath);
    if (status != 0) {
        fprintf(stderr, "Could not enable external scorer.\n");
        // xxx need to try-catch block (?), might not work - or init in init()
        //return 1;
    }

    // _sample_rate is used to set SFML sample rate, needs to match DS
    // sample rate
    assert(DS_GetModelSampleRate(_ctx) == _sample_rate);

    // first check if an input audio device is available on the system
    if (!sf::SoundBufferRecorder::isAvailable()) {
        std::cerr << "Audio capture is not available on this system.\n";
    }

    // xxx static_cast? what's the point of the enum, sort out better
    // (SEE: struct AudioBuffer instantiation)
    _recorder.setChannelCount(_buf.channels);

    /** @brief This should be addressed... */
    #if defined(__GNUC__) || defined(__GNUG__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wwrite-strings"
    #endif

    std::vector<char*> hotwords = {"up", "down", "left", "right"};

    //for (int i = 0; i < hotwords.size(); ++i) {
    //    // NOTE: Between 8-9-10 (thought 9 was ideal!).
    //    DS_AddHotWord(_ctx, hotwords.at(0), 9.f);
    //}

    //std::vector<char*> coldwords = {"but", "at", "he", "ah", "the", "put", "bu",
    //    "laughed", "ray", "lalage", "a", "raleigh", "lalala", "la", "lala",
    //    "let", "bound", "way", "ran", "can", "she", "that", "laplace",
    //    "legless", "leslie", "leaf", "it", "retract", "retreat", "laugh",
    //    "rare", "literate", "they", "then", "dan", "about", "reflect", "late",
    //    "letter", "after", "rat", "lefroy", "lefty", "we", "black", "laughter",
    //    "lara", "rare", "levi", "later", "tara", "well", "olaf", "bright",
    //    "are", "last", "breath", "best", "boundless", "westray", "repeat",
    //    "willet", "riles", "about", "to", "laughest", "latest", "levelest",
    //    "reflect", "little", "lest", "lachan", "lathrop", "have", "red",
    //    "rara"};

    ///** @note Close words that should also be parsed as command:
    // *     "let" -> "left"
    // *     "but" -> "up"
    // *     "a" -> "up"
    // *     "bright" -> "right"
    // *     "he" -> "up"
    // *     "at" -> "up" LOOK INTO?
    // *     "last" -> left"
    // *     "downright" -> ???
    // *     "ran" -> right" MAYBE?
    // *     "man" -> ??
    // *     "lest" -> "left"
    // *     "last" -> "left"
    // *     "upright" -> ???
    // */

    //for (int i = 0; i < coldwords.size(); ++i) {
    //    DS_AddHotWord(_ctx, coldwords.at(0), -100.f);
    //}

    #if defined(__GNUC__) || defined(__GNUG__)
        #pragma GCC diagnostic pop
    #endif

    /** @brief Set model beam width in SpeechToText constructor (is set to a low
     * value for faster decodes, don't need complicated sentence structure). */

    // xxx figure out best beam width. NOTE: 100 was a MASSIVE improvement to
    // decode speed
    int beam_width = 75;
    status = DS_SetModelBeamWidth(_ctx, beam_width);
    if (status != 0) {
        fprintf(stderr, "Could not set model beam width.\n");
    }

    // xxx mess with alpha/beta
    // how much to weight external scorer words
    int alpha = 0;
    // how much does scorer award word insertions
    int beta = 0;
    /** @note Probably want to use pre-set alpha/beta, DeepSpeech fine-tuned
     * to American English, but optional overrides can be set. */
    //status = DS_SetModelAlphaBeta(_ctx, alpha, beta);
    //if (status != 0) {
    //    fprintf(stderr, "Could not set model alpha/beta.\n");
    //}
}

/**
 * Public run method to run SpeechToText.
 */
void stt::SpeechToText::run()
{
    // run() was called, set _run to true and enter loop
    _run = true;
    while (_run) {
        // NOTE: if clear() sets _run to false, will break out of loop and exit
        // run()
        record();
        decode();
        parse();
    }
}

/**
 * Record audio buffer for DeepSpeech to decode.
 * @note Runs on a new thread, decode() and parse() DOES NOT.
 * @todo Threading should be addressed.
 */
void stt::SpeechToText::record() {
    // start the capture
    // NOTE: start(sampleRate = 44100), overwrite default to 16000 for DS
    std::cout << "Start recording\n";
    _recorder.start(16000);

    // probably want some kind of set interval, not ideal, but workable
    // ...a better solution is overlapping and catting buffers, or having an
    // active buffer that is stream ... DS needs a buf though ... so not sure
    // (?)

    // xxx need to decide on correct buf interval
    sleep(2.5);

    // stop the capture
    _recorder.stop();
    std::cout << "End recording\n";

    // retrieve the buffer that contains the captured audio data
    const sf::SoundBuffer& buf = _recorder.getBuffer();

    // need raw audio data
    // is vector<sf::Int16>
    // xxx convert sf::Int16 to short and std::vector to array - OK?
    _buf.samples = static_cast<const short*>(buf.getSamples());

    // is std::size_t
    _buf.count = static_cast<unsigned int>(buf.getSampleCount());

    /** @brief AudioBuffer is fully instantiated, SAFE to proceed
     * @note Each call of record() will overwrite AudioBuffer with a new
     * sf::AudioBuffer. */
}

void stt::SpeechToText::decode() {
    std::cout << "Start decoding\n";
    char* stt = DS_SpeechToText(_ctx, _buf.samples, _buf.count);
    std::cout << "End decoding\n";

    std::cout << stt << "\n";

    /** @brief Convert local C char* stt to CXX member std::string decoded. stt
     * is freed from memory, CXX std::string will handle memory management. */
    _decoded = stt;
    DS_FreeString(stt);
}

/**
 * Get DeepSpeech SpeechToText decoded std::string, for Key interface to take
 * over and provide public Key interface.
 * @return std::string decoded, decoded DeepSpeech SpeechToText string
 */
std::string stt::SpeechToText::get_decoded()
{
    return _decoded;
}

void stt::SpeechToText::parse()
{
    /** @brief Parse CXX string for key inputs. */
    std::istringstream in(_decoded);
    std::string parse;
    while (in >> parse) {
        if (parse == "up")
            _key_queue.push(Key::Up);
        else if (parse == "down")
            _key_queue.push(Key::Down);
        else if (parse == "left")
            _key_queue.push(Key::Left);
        else if (parse == "right")
            _key_queue.push(Key::Right);

        /** @note Close words that should also be parsed as command:
         *     "let" -> "left"
         *     "but" -> "up"
         *     "a" -> "up"
         *     "bright" -> "right"
         *     "he" -> "up"
         *     "at" -> "up" LOOK INTO?
         *     "last" -> left"
         *     "downright" -> ???
         *     "ran" -> right" MAYBE?
         *     "man" -> ??
         *     "lest" -> "left"
         *     "last" -> "left"
         *     "upright" -> ???
         */

        else if (parse == "but")
            _key_queue.push(Key::Up);
        else if (parse == "a")
            _key_queue.push(Key::Up);
        //else if (parse == "he") // NOTE: "he" is probably too common!
        //    _key_queue.push(Key::Up);
        else if (parse == "at")
            _key_queue.push(Key::Up);
        else if (parse == "let")
            _key_queue.push(Key::Left);
        else if (parse == "last")
            _key_queue.push(Key::Left);
        else if (parse == "left")
            _key_queue.push(Key::Left);
        else if (parse == "last")
            _key_queue.push(Key::Left);
        else if (parse == "laughed")
            _key_queue.push(Key::Left);
        else if (parse == "late")
            _key_queue.push(Key::Left);

        // AFTER CUSTOM SCORER:
        else if (parse == "le")
            _key_queue.push(Key::Left);

        else if (parse == "play")
            _key_queue.push(Key::Play);
        else if (parse == "exit")
            _key_queue.push(Key::Exit);

        else
            continue;
    }
}

void stt::SpeechToText::clear()
{
    std::queue<Key> empty;
    std::swap(_key_queue, empty);
    _run = false;
}

stt::Key stt::SpeechToText::get_key()
{
    stt::Key key = _key_queue.front();
    _key_queue.pop();
    return key;
}

bool stt::SpeechToText::key_queue_is_empty() const
{
    return _key_queue.empty();
}

/**
* DeepSpeech cleanup.
*/
stt::SpeechToText::~SpeechToText()
{
    DS_FreeModel(_ctx);
}
