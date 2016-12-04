/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HttpMonitor.h
 * Author: root
 *
 * Created on October 12, 2016, 6:11 PM
 */

#ifndef HTTPMONITOR_H
#define HTTPMONITOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "Logger.h"

class HttpMonitor {
public:
    HttpMonitor();
    HttpMonitor(const HttpMonitor& orig);
    HttpMonitor(std::string tid, std::string url, std::string port, int timeout, long cInterval);
    HttpMonitor & operator=(const HttpMonitor& orig);
    std::string getUrl() const;
    long  getInterval() const;
    int  getSock() const;
    void setSock(int sock);
    struct addrinfo*  getAInf() const;
    void setAInf(struct addrinfo* aInf);
    int openSocket();
    int maxFd(std::vector<HttpMonitor>) const;
    int checkHttp(std::vector<HttpMonitor>);
    long timeDiff(struct timeval * result, struct timeval * x, struct timeval * y);
    int headerReceived(std::string request);
    int getContentLength(std::string request);
    void parseUrl(std::string & host,  std::string & path);
    std::string trim(const std::string & str);
    bool makeConnection(std::vector<HttpMonitor> vsock);
    bool getResponce(int sock, std::string & responce, struct timeval& timeout);
    bool sendRequest(int sock, const std::string & domain, const std::string & path, struct timeval& timeout);
    
    virtual ~HttpMonitor();
public:

    std::string _tid;
    std::string _url;
    std::string _port;
    struct timeval _timeout;
    long _cInterval;
    time_t  nextcheck;

private:
    int  _sock;
    struct addrinfo * _aInf;
    Logger _logger;
    
};

#endif /* HTTPMONITOR_H */

