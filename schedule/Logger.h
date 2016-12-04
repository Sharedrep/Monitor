
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include<pthread.h>
#include<unistd.h>
#include<vector>
#include <ctime>
#include "HttpMonitor.h"

#ifndef LOGGER_H
#define LOGGER_H

class Logger {
public:
    Logger();
    Logger(const Logger& orig);
    virtual ~Logger();
    void addToLog(std::string loginfo);
private:

};

#endif /* LOGGER_H */

