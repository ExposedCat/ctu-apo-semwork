# Multiplayer Snake Game

# Description
"Snake" for two players, i.e. there are two snakes in the game, each for one of the
players. The goal of the player is to control their snake so that it collects as much food
as possible to achieve a higher score than opponent.

# Controls
The snake can only move up, down, left and right. The knob can be turned one degree
to the left or right. If the position of the knob does not change, the snake continues in
the previous direction.

- Change direction for snake 1 (left): rotate Red Knob
- Change direction for snake 2 (right): rotate Blue Knob


# Build instructions
* Clear previous build using `make clean`
* Set board local IP address as environment variable `TARGET_IP`, either
    * by specifying it before next step `TARGET_IP=192.168.* ...`
    * or setting it globally for current shell `export TARGET_IP=192.168.*`
* Run game using `make run` (note: see previous step)
