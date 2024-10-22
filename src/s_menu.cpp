#include "s_menu.h"
#include "utility.h"
#include "r_holders.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include <pthread.h>

#include <iostream>
#include <thread>

MenuState::MenuState(StateStack& stack, Context context) :
    State(stack, context),
    m_options(),
    m_options_index(0),
    _stt(std::make_unique<stt::SpeechToText>()),
    _stt_start(true),
    _th()
{
    // get title screen from context, already in mem, don't need to recreate
    m_background_sprite.setTexture(context.textures->get(
                Textures::TitleScreen));
    scale_sprite(m_background_sprite);
    // get main font from context, already in mem, don't need to recreate
    sf::Font& font = context.fonts->get(Fonts::Main);

    // MENU:
    // set starting y position and option offset (from start)
    int start = 632;
    int offset = 35;

    // play option...
    sf::Text play_option;
    play_option.setFont(font);
    play_option.setString("Play");
    center_origin(play_option);
    // (x, y) px of play
    play_option.setPosition(640, start);
    // add to menu options vector
    m_options.push_back(play_option);

    // settings option...
    sf::Text settings_option;
    settings_option.setFont(font);
    settings_option.setString("Settings");
    center_origin(settings_option);
    // (x, y) px of exit, -30 px y offset from play
    settings_option.setPosition(640, start + offset);
    // add to menu options vector
    m_options.push_back(settings_option);

    // exit option...
    sf::Text exit_option;
    exit_option.setFont(font);
    exit_option.setString("Exit");
    center_origin(exit_option);
    // (x, y) px of exit, -30 px y offset from settings
    exit_option.setPosition(640, start + (2 * offset));
    // add to menu options vector
    m_options.push_back(exit_option);

    update_option_text();
}

void MenuState::draw()
{
    // get context of window, already in mem, don't recreate
    sf::RenderWindow& window = *get_context().window;

    // draw menu screen and set view
    window.setView(window.getDefaultView());
    window.draw(m_background_sprite);
    // for each menu option draw
    for (const sf::Text& text : m_options)
        window.draw(text);
}

/**


 * Update cycle for menu state.
 * @param sf::Time delta_time (game delta time).
 * @return Returns true to keep previous state updating.
 * @remark Delta time is a parameter for each state's update() - unused if not
 * needed! Like in a menu...!
 */
bool MenuState::update(sf::Time delta_time)
{
    //if (_stt_start) {
    //    _th = std::thread ([this]() { _stt->run(); });
    //    _th.detach();
    //    _stt_start = false;
    //}

    // stt handling
    // @note stt is in update loop, to circumvent handle_event() - should take
    // higher precedence (like realtime input)
    //if (!_stt->key_queue_is_empty()) {
    //    _stt_key = _stt->get_key();
    //    if (_stt_key == stt::Key::Play) {
    //        _stt->clear();
    //        std::cout << "Stopping SpeechToText...\n";
    //        _stt.reset();
    //        //pthread_cancel(_th.native_handle());
    //        //std::cout << "SpeechToText Linux pthread killed.\n";
    //        //request_pop_stack();
    //        std::cout << "Requesting GameState...\n";
    //        request_push_stack(States::Game);
    //    } else if (_stt_key == stt::Key::Exit) {
    //        request_pop_stack();
    //    } else {
    //        // @note maybe not necessary, because new key will be popped, but
    //        // just in case...!
    //        _stt_key == stt::Key::None;
    //    }
    //}

    return true;
}

/**
 * Handle events for menu state.
 * @param sf::Event& event (to be handled).
 * @return Returns false to keep previous state from handling events.
 */
bool MenuState::handle_event(const sf::Event& event)
{
    // logic for each option...
    if (event.type != sf::Event::KeyPressed)
        return false; // don't handle event if no key is pressed!
    if (event.key.code == sf::Keyboard::Return) {
        if(m_options_index == Play) {
            // if play is pressed ->
            // pop off menu state & push game state to top of stack
            request_pop_stack();
            request_push_stack(States::Game);
        } else if (m_options_index == Settings) {
            request_push_stack(States::Settings);
        } else if (m_options_index == Exit) {
            // if exit is pressed ->
            // pop off menu state -> stack will be empty and game will close...
            request_pop_stack();
        }
    } else if (event.key.code == sf::Keyboard::Up) {
        // decrement option and wrap-around
        if (m_options_index > 0)
            --m_options_index;
        else
            m_options_index = m_options.size() - 1;
        update_option_text();
    } else if (event.key.code == sf::Keyboard::Down) {
        // increment option and wrap-around
        if (m_options_index < m_options.size() - 1)
            ++m_options_index;
        else
            m_options_index = 0;
        update_option_text();
    }

    return false;
}

void MenuState::update_option_text()
{
    // occ colors:
    sf::Color occ_blue(0, 45, 106);
    sf::Color occ_orange(249, 146, 57);

    // guard action in case options empty...
    if (!m_options.empty()) {
        // occ blue all option text
        for (sf::Text& text : m_options)
            text.setFillColor(occ_blue);
        // occ orange selected option text
        m_options[m_options_index].setFillColor(occ_orange);
        // xxx changes blue options to have outline too, after select (!)
        //m_options[m_options_index].setOutlineColor(sf::Color::Black);
        //m_options[m_options_index].setOutlineThickness(2.5f);
    }
}

MenuState::~MenuState()
{
    std::cout << "Menu state has been destroyed!" << std::endl;
}
