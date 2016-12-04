#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <fstream>
#include<pthread.h>
#include<unistd.h>
#include <fcntl.h> 
#include <sstream>
#include<vector>
#include<map>
#include <sys/time.h>
#include "HttpMonitor.h"
#include "Logger.h"

#ifndef SENDER_H
#define SENDER_H

class Sender {
public:
    Sender();
    Sender(const Sender& orig);
    virtual ~Sender();

    bool sendRequest();
    void AddMonitor(HttpMonitor * pmon);
    void maxFd(int fd) ;
    long minTimeout() const;
    long timeDiff(struct timeval * result, struct timeval * x, struct timeval * y);
private:

    std::vector<HttpMonitor>* testlist;
    int _maxFd;

    std::multimap<time_t, HttpMonitor*> testMap;

public:
    Logger _logger;
};

#endif /* SENDER_H */

