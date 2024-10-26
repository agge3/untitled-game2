/**
 * @project untitled game
 * @copyright See LICENSE in the top level directory
 * @file main.cpp
 * @purpose A fun and untitled game
 * @author agge3
 */

#include "app.h"

#include <gtest/gtest.h>

#include <stdexcept>
#include <iostream>

int main(int argc, char **argv)
{
    try {
        Application app;
        app.run();
    } catch (std::exception& e) {
        std::cerr << "\nexception: " << e.what() << std::endl;
    }

	return 0;
}
