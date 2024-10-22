#pragma once

//#define SFML_STATIC

#include "state.h"
#include "speech-to-text.h"
#include "task-thread.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <memory>
#include <thread>

class MenuState : public State {
public:
    MenuState(StateStack& stack, Context context);
    ~MenuState();

    virtual void draw();
    virtual bool update(sf::Time delta_time);
    virtual bool handle_event(const sf::Event& event);

    void update_option_text();
private:
    enum Option {
        Play,
        Settings,
        Exit,
    };

    sf::Sprite m_background_sprite;
    std::vector<sf::Text> m_options;
    std::size_t m_options_index;

    /** Members for SpeechToText functionality. */
    std::unique_ptr<stt::SpeechToText> _stt;
    TaskThread _stt_task;
    stt::Key _stt_key;
    bool _stt_start;
    std::thread _th;
};
