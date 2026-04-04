#include <iostream>
#include <stdexcept>
#include <string>

#include "nanodb/command_dispatcher.hpp"
#include "nanodb/nanodb.hpp"

int main() {
    NanoDB db;
    CommandDispatcher dispatcher(db);

    std::string line;

    while (true) {
        std::cout << "> ";

        if (!std::getline(std::cin, line)) break; // EOF (Ctrl+D)

        if (line.empty()) continue;
        if (line == "exit" || line == "quit") break;

        try {
            std::string result = dispatcher.dispatch(line);
            std::cout << result << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }
}