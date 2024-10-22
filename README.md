# OCC Accessibility Tour
## Gile's T. Brown Research Symposium

The normal video game controls of mouse and keyboard (or controller) are entirely exclusive to people of ability. Welcome OCC Accessibility Tour, where we
aim to bridge that gap, and unite video gaming with people from all abilities. With a modular library set, we have created a game that can be played entirely
by voice alone. 

In its current state, it is a creative rendition of Orange Coast College's Map, where the player, Pete the Pirate, can explore OCC.

The design is to have the library set up in a way that it can be ported to any game.

### To compile (currently Linux only)
  1. Clone into the repository
  2. In root, <code>mkdir build</code>
  3. <code>cd build</code>
  4. <code>cmake ../</code>
  5. <code>make occ-accessibility-tour</code>
  6. <code>./occ-accessibility-tour</code>

### Things to do:
  - Cross-compatibility
  - Menu voice operations
  - Continuous speech transmission & translation
  - Decide on whether to continue the current game, or create a better game suited to the task

## Credit: 
Professor Cole, Mentor<br>
[SFML](https://www.sfml-dev.org/)<br>
[Mozilla's DeepSpeech C API](https://deepspeech.readthedocs.io/en/latest/C-API.html)<br>
Documentation auto-generated with Doxygen<br>

## Authors:
@agge3<br>
@kpowkitty
