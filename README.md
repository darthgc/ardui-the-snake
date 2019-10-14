# ardui-the-snake
Arduino-based Snake game. The snake's name is Ardui!

README is WIP

# Getting Started
VS Code was used as the IDE. The VS Code extension [Arduino](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino) was used as well in order to verify and upload the sketch.

Install [LedControl](https://github.com/wayoda/LedControl/releases) in your Arduino libraries.

Install Visual Studio 2017 C++ compiler (see .vscode/c_cpp_properties.json).

The method `getNode` from the LinkedList library had to be modified a bit, otherwise it was not compiling with the compiler that this project is using.

# How to use
If you are using VS Code, use the `Ctrl+Shift+P` shortcut and type+select `Arduino: Upload`.
