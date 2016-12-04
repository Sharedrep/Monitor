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

#ifndef CONNECTOR_H
#define CONNECTOR_H

class Connector {
public:
    Connector();
    Connector(const Connector& orig);
    virtual ~Connector();

    bool makeConnection();
    void AddMonitor(HttpMonitor * pmon);
    void maxFd(int fd) ;
    long minTimeout() const;
    long timeDiff(struct timeval * result, struct timeval * x, struct timeval * y);
private:
    std::vector<HttpMonitor>* testlist;
    int _maxFd;

public:
    Logger _logger;
};

#endif /* CONNECTOR_H */

