#pragma once

//#define SFML_STATIC

#include "command.h"
#include "task-thread.h"

#include "speech-to-text.h"

#include <SFML/Window/Event.hpp>
#include <SFML/System/Vector2.hpp>

#include <map>
#include <memory>

extern sf::Vector2f PREV_PLAYER_MOVEMENT;

/** @brief Forward definition of CommandQueue to be used in implementation. */
class CommandQueue;

/**
 * @class Player
 * Controls input handling of Player Creature.
 */
class Player {
public:
    Player();

    /**
     * @enum Action
     * Player actions - to allow keybinding.
     */
    enum Action : unsigned int {
        None = 0,
        MoveUp = 1 << 0,
        MoveDown = 1 << 1,
        MoveLeft = 1 << 2,
        MoveRight = 1 << 3,
        MeleeAttack = 1 << 4,
        RangedAttack = 1 << 5,
        MagicAttack = 1 << 6,
        SpecialAttack = 1 << 7,
        Inventory = 1 << 8,
        Map = 1 << 9,
        STTMoveUp = 1 << 10,
        STTMoveDown = 1 << 11,
        STTMoveLeft = 1 << 12,
        STTMoveRight = 1 << 13,
        STTExit = 1 << 14,
        STTPlay = 1 << 15,
    };

    /**
     * @enum LevelStatus
     * Current level status.
     */
    enum LevelStatus : unsigned int {
        /** @todo Migrate to enum class. */
        //None = 0,
        InProgress = 1 << 0,
        Success = 1 << 1,
        Failure = 1 << 2,
    };

    // for one-time key presses
    void handle_event(const sf::Event& event, CommandQueue& commands);
    // for real-time input
    void handle_realtime_input(CommandQueue& commands);
    // for local voice lib
    void handle_stt_input(CommandQueue& commands);
    // fn to bind keys and get assigned keys
    void assign_key(Action action, sf::Keyboard::Key key);
    sf::Keyboard::Key get_assigned_key(Action action) const;
    void set_level_status(LevelStatus status);
    LevelStatus get_level_status() const;
    /** @todo Returns nullptr until implemented. */
    char* print_assigned_key(Action action) const;
    void run_stt();
    bool is_stt_running();

private:
    void initialize_actions();
    static bool is_realtime_action(Action action);

    /**
     * @var std::map<sf::Keyboard::Key, Action> m_keybinding
     * Key to bind to Action.
     * @note Store the keys and actions in map together instead of as seperate
     * variables.
     */
    std::map<sf::Keyboard::Key, Action> m_keybinding;

    /**
     * @var std::map<stt::Key, Action> _sttbinding
     * stt::Key to bind to Action.
     * @note Store the keys and actions in map together instead of as seperate
     * variables.
     */
    std::map<stt::Key, Action> _sttbinding;

    /**
     * @var std::map<Action, Command> m_actionbinding
     * Action to bind to Command.
     * @note Store the actions and commands in a map together instead of as
     * seperate variables.
     */
    std::map<Action, Command> m_actionbinding;

    /**
     * @var LevelStatus m_current_level_status
     * Holds current LevelStatus of Player.
     */
    LevelStatus m_current_level_status;

    /**
     * @var std::unique_ptr<stt::SpeechToText> _stt
     * Instantiate SpeechToText as std::unique_ptr and bind to Player.
     */
    std::unique_ptr<stt::SpeechToText> _stt;

    /**
    * @var stt::Key _stt_key
    * SpeechToText Key for use as Player key input.
    */
    stt::Key _stt_key;

    TaskThread _stt_task;
};
