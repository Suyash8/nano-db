#pragma once

#include <cstdint>
#include <sstream>
#include <string>

#include "nanodb.hpp"

class CommandDispatcher {
    NanoDB& db_;

public:
    CommandDispatcher(NanoDB& db) : db_(db) {}

    std::string dispatch(std::string command_line) {
        std::stringstream ss(command_line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "insert") {
            int64_t ts;
            double val;

            if (ss >> ts >> val) {
                db_.insert(ts, val);
                return "OK";
            } else {
                return "Usage: insert <timestamp> <value>";
            }
        } else if (cmd == "query") {
            int64_t start, end;

            if (ss >> start >> end) {
                auto results = db_.query(start, end);

                if (results.empty()) {
                    return "(no results)";
                } else {
                    std::string output;
                    for (const auto& [ts, val] : results)
                        output += std::to_string(ts) + ": " + std::to_string(val) + "\n";
                    return output;
                }
            } else {
                return "Usage: query <start> <end>";
            }
        } else if (cmd == "save") {
            std::string filename;

            if (ss >> filename) {
                db_.save(filename);
                return "OK";
            } else {
                return "Usage: save <filename>";
            }
        } else if (cmd == "load") {
            std::string filename;

            if (ss >> filename) {
                db_.load(filename);
                return "OK";
            } else {
                return "Usage: load <filename>";
            }
        } else if (cmd == "stats") {
            return "Blocks: " + std::to_string(db_.getBlockCount());
        } else if (cmd == "help") {
            return "Commands:\n"
                   "  insert <timestamp> <value>\n"
                   "  query <start> <end>\n"
                   "  save <filename>\n"
                   "  load <filename>\n"
                   "  stats\n"
                   "  help\n"
                   "  exit/quit";
        } else {
            return "Unknown command. Type 'help' for a list of commands.";
        }
    }
};