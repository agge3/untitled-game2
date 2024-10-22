#include "s_game.h"

#include <iostream>
#include <functional>

GameState::GameState(StateStack& stack, Context context) :
    State(stack, context),
    m_world(*context.window, *context.fonts),
    m_player(*context.player),
    _stt_start(true)
{
    // print successful state creation
    std::cout << "Game state created!\n";
    //std::thread _stt_thread(&stt_thread);
    //_stt_thread.detach();
}

void GameState::draw()
{
    m_world.draw();
}

bool GameState::update(sf::Time delta_time)
{
    // xxx game state is updating, meaning stt should be running...
    // run on separate thread and don't run again until thread has completed
    if (_stt_start) {
        m_player.run_stt();
        std::cout << "Creating thread...\n";
        _stt_start = false;
    }

    /// Update and realtime input done in same update cycle of game state.
    m_world.update(delta_time);
    /// Get commands from command queue, then handle input.
    CommandQueue& commands = m_world.get_command_queue();
    m_player.handle_realtime_input(commands);
    m_player.handle_stt_input(commands);
    // uncomment to print if game update loop is handling realtime input
    //std::cout << "Game update loop: Receiving realtimesttt!\n";

    return true;
}

bool GameState::handle_event(const sf::Event& event)
{
    // game input handling
    CommandQueue& commands = m_world.get_command_queue();
    m_player.handle_event(event, commands);

    /// If escape pressed, push pause state as current state.
    if (event.type == sf::Event::KeyPressed
            && event.key.code == sf::Keyboard::Escape) {
        /** @brief Clear SpeechToText context, new context will be used each 
         * time game state becomes the active state. */
        //m_player.clear_stt();

        request_push_stack(States::Pause);
    }

    return true;
}

GameState::~GameState()
{
    std::cout << "Game state has been destroyed!" << std::endl;
}

/*
// unused from SFML game dev book
// update fps, etc.
void Game::update_statistics(sf::Time elapsed_time)
{
    m_statistics_update_time += elapsed_time;
    m_statistics_num_frames += 1;

    if (m_statistics_update_time >= sf::seconds(1.0f)) {
        m_statistics_text.setString("Frames / Second = " +
            toString(m_statistics_num_frames) + "\n" + "Time / Update = " +
            toString(m_statistics_update_time.asMicroseconds() /
            m_statistics_num_frames + "us"));

        m_statistics_update_time -= sf::seconds(1.0f);
        m_statistics_num_frames = 0;
    }
}
*/
