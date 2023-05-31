#include "log.hpp"

void Log::Print(int status, const std::string &message, std::ostream &fout) {

    /*
        Information:
        1. Thread id
        2. Type of message
        3. Message
    */

    fout << "\n[" << std::this_thread::get_id() << "] [" << "] [" << ResolveStatus(status) << "] " << message << '\n';
}

std::string Log::ResolveStatus(int status) {
        switch (status)
        {
        case ERROR:
            return "ERROR";
        case WARNING:
            return "WARNING";
        case INFO:
            return "INFORMATION";
        case DEBUG:
            return "DEBUG";
        }
        return "";
}
