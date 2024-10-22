1. run daemon and poll
2. keep open sound buffer
3. send real-time to ds and process buffer
    * needs to be connected to deepspeech to proccess buffer on-demand
NOTE: need some way to throw away garbage and collect only desired words

4. parse stt and map to keybinds - prolly a header that can be used
PARSER: when hits keyword hook forwards it as a cmd
    * maybe need a daemon for the parser... (?) - until EOF, const parses file
      and forwards keyword hooks

* clean interface, should map to easily callable binds
* just focus on up,down,left,right




# RECORDING
sf::SoundRecorder -> records audio (usually sends to a buffer, DS wants a buf!)

sf::SoundRecorderBuffer -> what we're looking for, proc for DS

SpeechToText...
    (1) initializes (in default constructor)
    
    run public method...
        (1) record()
        (2) decode()
        (3) parse()
            * need to "get" KeyQueue, to be processed in game loop
                * KeyQueue and Key should be interface ... how to organize?

# Design Update:
SpeechToText class is internal and does all necessary functions.
    * records
    * decodes
    * parses
        -> inserts into KeyQueue

stt-api is public API and allows access of SpeechToText in an easy-to-use 
interface.

## Need:
__Control flow:__
    1. Initialize SpeechToText, and run record(), decode(), parse() on repeat
        -> insert Key(s) into KeyQueue
    NOTE: At this point, public or private interface?
    2. Check if KeyQueue has Key(s) in queue
    3. pop() the next Key and send associated action to CommandQueue (each 
       update cycle)

__Questions:__
    * Public API or just let SpeechToText be the public API?

* Enum definition of Key(s)

## REVISION:
* SpeechToText class does the heavy lifting.
* run()/init() to run SpeechToText
    * NEED: seperate thread for DeepSpeech decode... (!)
* Don't want to have to call in-class scope, would prefer an API to feed Key(s)
  and certain key functions
    * 2 choices: Have an API header that wrappers a bunch of stuff, or have one
      speech-to-text.h, which just has everything in it
        * API header: Looks nice, will have to pass around context (maybe pia),
                      maybe weird/awkward?
        * speech-to-text.h: Going to have a bunch of stuff in it, so pretty 
                            bloated header/maybe not a clean interface, but why
                            wrapper what doesn't need to be wrapped when the
                            class does everything anyway.
      
      __Answer:__ Pass around SpeechToText context, especially with threading 
                  seems OK, would decouple it quite a bit.

                  Wonder if there's a way to just run SpeechToText entirely on
                  its own thread, remove the threading for sf::SoundRecorder
                  and just pass around context, that can grab the queue if
                  necessary...

SpeechToText SHOULD: record, decode, parse -> output meaningful speech-to-text
    * NEEDS to run on separate thread, and persist... Just grab context and 
      get queue. Need a way to reset context if game state meaningfully changes
        -> Pretty much if game state is not current state, clear speech-to-text
           context

SO, game state needs to create SpeechToText context, which player can draw from.
Need to pass in context pretty much via game state

Bind SpeechToText context to Player (since it is part of Player)
    -> Player needs a way to communicate SpeechToText context to states
        __Answer:__ Wrapper functions in Player
