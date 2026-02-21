#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "nanodb/nanodb.hpp"

int main() {
    NanoDB db;

    std::string line;

    while (true) {
        std::cout << "> ";

        if (!std::getline(std::cin, line)) break; // EOF (Ctrl+D)

        if (line.empty()) continue;
        if (line == "exit" || line == "quit") break;

        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        try {
            if (cmd == "insert") {
                int64_t ts;
                double val;

                if (ss >> ts >> val) db.insert(ts, val);
                else std::cout << "Usage: insert <timestamp> <value>\n";
            } else if (cmd == "query") {
                int64_t start, end;

                if (ss >> start >> end) {
                    auto results = db.query(start, end);

                    if (results.empty()) {
                        std::cout << "(no results)\n";
                    } else {
                        for (const auto& [ts, val] : results)
                            std::cout << ts << ": " << val << "\n";
                    }
                } else {
                    std::cout << "Usage: query <start> <end>\n";
                }
            } else if (cmd == "save") {
                std::string filename;

                if (ss >> filename) db.save(filename);
                else std::cout << "Usage: save <filename>\n";
            } else if (cmd == "load") {
                std::string filename;

                if (ss >> filename) db.load(filename);
                else std::cout << "Usage: load <filename>\n";
            } else if (cmd == "stats") {
                std::cout << "Blocks: " << db.getBlockCount() << "\n";
            } else if (cmd == "help") {
                std::cout << "Commands:\n";
                std::cout << "  insert <timestamp> <value>  Insert a new data point\n";
                std::cout << "  query <start> <end>         Query data points in range\n";
                std::cout << "  save <filename>             Save database to file\n";
                std::cout << "  load <filename>             Load database from file\n";
                std::cout << "  stats                       Show database statistics\n";
                std::cout << "  exit / quit                 Exit the shell\n";
            } else {
                std::cout << "Unknown command '" << cmd << "'. Type 'help' for a list of commands.\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }
}