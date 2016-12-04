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

#ifndef TESTLIST_H
#define TESTLIST_H

class Testlist {
public:
    Testlist();
    Testlist(const Testlist& orig);
    virtual ~Testlist();

    void waitCInterval(time_t &sleeptime , HttpMonitor* &pmon);
    void  getTests();
    long timeDiff(struct timeval * result, struct timeval * x, struct timeval * y);
private:

    std::multimap<time_t, HttpMonitor*> testMap;

public:
    Logger _logger;
};

#endif /* TESTLIST_H */

