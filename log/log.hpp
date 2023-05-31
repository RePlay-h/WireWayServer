#ifndef WireWayServer_LOG_H
#define WireWayServer_LOG_H

#include <fstream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <iostream>

#define ERROR 0
#define INFO 1
#define WARNING 2
#define DEBUG 3

/*
    The file contains functions for logging output
*/

namespace Log {

    void Print(int status, const std::string& message, std::ostream &fout);
    
    std::string ResolveStatus(int status); // returns the message type text

} // log

#endif