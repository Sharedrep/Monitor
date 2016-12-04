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

#ifndef ADDRGETTER_H
#define ADDRGETTER_H

class Addrgetter {
public:
    Addrgetter();
    Addrgetter(const Addrgetter& orig);
    virtual ~Addrgetter();

    void GetAddrinfo(); 
    int  getHostByName(HttpMonitor * pmon);
    void AddMonitor(HttpMonitor*& pmon);
    void RemoveMonitor(HttpMonitor * pmon);

private:
    void maxFd(int fd) ;
    long minTimeout() const;
    long timeDiff(struct timeval * result, struct timeval * x, struct timeval * y);

    int _maxFd;
    std::map<HttpMonitor*, gaicb*> testMap;
    std::map<gaicb*, HttpMonitor*> hostMap;
    struct gaicb **reqs ;
    int nreqs;


public:
    Logger _logger;
};

#endif /* ADDRGETTER_H */

