#pragma once

#include "category.h"

#include <SFML/System/Time.hpp>

#include <functional>
#include <cassert>

class SceneNode;

struct Command {
    // treat function as object
    typedef std::function<void(SceneNode& node, sf::Time dt)> Action;

    Command();

    Action action;
    // store the recipients of the command in a category
    unsigned int category;
};

template <typename GameObject, typename Function>
Command::Action derived_action(Function fn)
{
    return [=] (SceneNode& node, sf::Time dt) {
        // check if cast is safe
        assert(dynamic_cast<GameObject*>(&node) != nullptr);
        // downcast node and invoke function on it
        fn(static_cast<GameObject&>(node), dt);
    };
}
