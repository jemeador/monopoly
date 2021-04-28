# Monopoly in C++

![Screenshot from the Monopoly web interface](docs/monopoly.png?raw=true "A project to finish that Monopoly game faster")

This is a personal project for practicing development outside of work. We ask
interview candidates how they would design Monopoly, so I thought I'd give it a shot
(despite how terrible it is as a board game). My goal was to practice the following:

- Newer C++ features I haven't gotten to use (C++14 and greater)
- CMake
- Clean code practices
- Clean user interfaces
- Test driven development

It has since expanded to include:

- Behavior driven development
- Web Assembly / Emscripten
- Javascript

I will provide more details later, stay tuned!

## To Do

These represent the minimum features before I can consider this project complete:

Web interface
- Trade UI
Engine
- Add a player action window for unmortgaging properties after acquisition (to avoid double interest payments)
- Add a player action window for building after gaining money
Server
- Multiplayer lobbies (currently hot-seat only)

## Building

Requires CMake 3.

I'm currently using Visual Studio 2019 on Windows for development, but it builds on Ubuntu.

If you would like to build WebAssembly with Emscripten, you will need emsdk checked out separately. It includes a CMake toolchain that can be used to compile this project.

More instructions coming soon.

## Usage

If you can manage to build the project with emscripten, you can start the server.js under buld.emscripten with Node.js and connect to it from a browser to play Monopoly!
You can also play at http://www.jeremymeador.com (once it becomes available)

## Contributing
This is currently for my own personal practice, but you are free to fork and play with it yourself. The engine is designed to be used for any interface (command line, AI, web, desktop, whatever)