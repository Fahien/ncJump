[![Linux](https://github.com/Fahien/ncJump/workflows/Linux/badge.svg)](https://github.com/Fahien/ncJump/actions?workflow=Linux)
[![macOS](https://github.com/Fahien/ncJump/workflows/macOS/badge.svg)](https://github.com/Fahien/ncJump/actions?workflow=macOS)
[![Windows](https://github.com/Fahien/ncJump/workflows/Windows/badge.svg)](https://github.com/Fahien/ncJump/actions?workflow=Windows)
[![MinGW](https://github.com/Fahien/ncJump/workflows/MinGW/badge.svg)](https://github.com/Fahien/ncJump/actions?workflow=MinGW)
[![Emscripten](https://github.com/Fahien/ncJump/workflows/Emscripten/badge.svg)](https://github.com/Fahien/ncJump/actions?workflow=Emscripten)
[![Android](https://github.com/Fahien/ncJump/workflows/Android/badge.svg)](https://github.com/Fahien/ncJump/actions?workflow=Android)
[![CodeQL](https://github.com/Fahien/ncJump/workflows/CodeQL/badge.svg)](https://github.com/Fahien/ncJump/actions?workflow=CodeQL)

# ncJump

[ncJump](https://github.com/Fahien/ncJump) is a project started on December 2020 with the purpose of learning and testing the capabilities of a new engine: [nCine](https://github.com/nCine/nCine), an open-source C++ framework for 2D games developed by [@encelo](https://github.com/encelo).

Some time ago, a bunch of members of the [GameLoop](https://gameloop.it) community followed [CS50's introduction to game development](https://cs50.harvard.edu/games/2018/), and I though the sequence of games presented in that course could give me a good insight on what kind of game to create.

A couple of developers already made or started to make the first few games listed by that course. There is [a Pong clone](https://github.com/nCine/ncPong) made by @encelo, a Flappy Bird clone made by @Vasile-Peste, and a Breakout clone under development by @mat3. I opted for a Super Mario Bros clone.

The idea is to follow the steps of [CS50's lecture 4](https://cs50.harvard.edu/games/2018/weeks/4/) by using nCine instead of [Love2D](https://love2d.org/).

## Getting started

Some steps are required in order to *ncJump-start*, but luckily everything you need to know is documented on the [nCine download web-page](https://ncine.github.io/download/):

  1. Install nCine and keep note of its _build_ directory.
     ```sh
     NCINE_BUILD_DIR=<you/know>
     ```
  2. Clone this repository.
     ```sh
     git clone https://github.com/Fahien/ncJump.git && cd ncJump
     ```
  3. Initialize the data submodule
     ```sh
     git submodule update --init
     ```
  4. Generate the project.
     ```sh
     cmake -GNinja -S. -Bbuild \
         -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
         -DCMAKE_BUILD_TYPE=Debug \
         -DnCine_DIR=$NCINE_BUILD_DIR
     cmake --build build
     ```
  5. Start ncJumping!
     ```sh
     build/ncjump
     ```

## Demo

Every push to the master branch triggers Emscripten deployment to [this demo web-page](https://antoniocaggiano.eu/ncJump-artifacts). Please, [open an issue](https://github.com/Fahien/ncJump/issues/new) if you find any bug!

![Random screenshot](https://repository-images.githubusercontent.com/320819435/d06a2400-5b8f-11eb-8a8d-47c647e1509a)

## Devlogs

A bunch of devlogs are published periodically in a GitHub discussion within the nCine project, and I highly recommend reading them to get an insight of the development history of the project together with the overall architectural design.

  1. [Writing down some code](https://github.com/nCine/nCine/discussions/10#discussion-1181968)
  2. [Everything is entity](https://github.com/nCine/nCine/discussions/10#discussioncomment-259064)
  3. [The smooth and the file](https://github.com/nCine/nCine/discussions/10#discussioncomment-272772)
  4. [Jump and resize](https://github.com/nCine/nCine/discussions/10#discussioncomment-288998)
  5. [Dynamics and destructibles](https://github.com/nCine/nCine/discussions/10#discussioncomment-319308)
  6. [Push and pull](https://github.com/nCine/nCine/discussions/10#discussioncomment-360424)
  7. [Enemies, factories, commands, and scripts](https://github.com/nCine/nCine/discussions/10#discussioncomment-381645)
  8. [Dying and deploying](https://github.com/nCine/nCine/discussions/10#discussioncomment-500765)

## License

The source code is licensed under the [MIT License](LICENSE).
