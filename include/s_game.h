#pragma once

//#define SFML_STATIC

#include "state.h"
#include "world.h"
#include "player.h"
#include "task-thread.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

class GameState : public State {
public:
    GameState(StateStack& stack, Context context);
    ~GameState();

    virtual void draw();
    virtual bool update(sf::Time delta_time);
    virtual bool handle_event(const sf::Event& event);
    // rendering handled by app
private:
    World m_world;
    Player& m_player; // player& -> (may be) already loaded in mem
    
    /**
    * @var TaskThread _stt_task
    * Thread to async run SpeechToText on.
    */
    TaskThread _stt_task;
    
    bool _stt_start;
};
