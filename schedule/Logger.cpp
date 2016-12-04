/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "Logger.h"


Logger::Logger() {
}

Logger::Logger(const Logger& orig) {
}

Logger::~Logger() {
}


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void Logger::addToLog(std::string loginfo) {

    int tid, timeout, cinterval;
    size_t pos = 0;
    pthread_mutex_lock( &mutex );
    std::ofstream file;
    file.open ("log", std::ios::app);

    std::time_t t = std::time(0) ;
    char cdate[128] ;
    std::strftime( cdate, sizeof(cdate), " %x %X", std::localtime(&t) ) ;
    file <<"["<<cdate <<"] " << loginfo;

    file.close();
    pthread_mutex_unlock( &mutex );
    
}    
