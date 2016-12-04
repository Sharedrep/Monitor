/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HttpMonitor.cpp
 * Author: root
 * 
 * Created on October 12, 2016, 6:11 PM
 */

#include <iostream>                                                                                 
#include <string>
#include <string.h> 
#include <stdio.h>                                                                                   
#include <stdlib.h>  
#include "HttpMonitor.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h> 
#include <unistd.h>
#include <cerrno>
#include <sys/time.h> 
#include <regex>

HttpMonitor::HttpMonitor() {
}

HttpMonitor::HttpMonitor(const HttpMonitor& orig) {

  *this = orig;
  //  _tid = orig._tid;
  //  _url=orig._url;
  //  _port = orig._port;
  //  _timeout = orig._timeout;
  //  _cInterval = orig._cInterval;
}

HttpMonitor& HttpMonitor::operator=(const HttpMonitor& orig) {
    if(this == &orig)
        return *this;
    _tid = orig._tid;
    _url = orig._url;
    _port = orig._port;
   _timeout = orig._timeout;
   _cInterval = orig._cInterval;
   nextcheck = orig.nextcheck;
   _sock = orig._sock;
   _aInf = orig._aInf;
    return * this;
}

HttpMonitor::HttpMonitor(std::string tid, std::string url, std::string port, int timeout, long cInterval) :_tid(tid),  _url(url), _port(port), _timeout({timeout, 0}), _cInterval(cInterval), _sock(-1), _aInf(NULL){
}

std::string HttpMonitor::getUrl() const {
    return _url;
}

int  HttpMonitor::getSock() const {
    return _sock;
}

void  HttpMonitor::setSock( int sock) {
     _sock = sock;
}

struct addrinfo* HttpMonitor::getAInf() const {
    return _aInf;
}

void  HttpMonitor::setAInf(struct addrinfo* aInf) {
     _aInf = aInf;
}

HttpMonitor::~HttpMonitor() {
       if (_aInf != NULL) {
         freeaddrinfo(_aInf);
         _aInf = NULL;
       }
    if (_sock != -1){
        close(_sock);
    }
}

int HttpMonitor::openSocket() {
    struct addrinfo hints;
    struct addrinfo * res = NULL;
    int sock = -1;
    int status = -1;
    int ret, rc;
    struct addrinfo * aInf= NULL;
    long arg;
    bool fail = false;
    fd_set fset;
    std::string path ;
    std::string domain ;
    parseUrl(domain, path);

    while ( true ){ 
        
        struct timeval startTime, endTime, result; 

        memset(&hints, 0, sizeof hints); 

        int host = getaddrinfo(domain.c_str(), _port.c_str(), &hints, &res);
        if ( host != 0){
            _logger.addToLog("testing( "+ _tid + ") " + _url + " NOK -- Host: " + gai_strerror(host) + " \n");
            break;
        }

        for (aInf = res; aInf; aInf = aInf->ai_next) {
            gettimeofday(&startTime, NULL);
            sock = socket(aInf->ai_family, aInf->ai_socktype,aInf->ai_protocol);

            gettimeofday(&endTime, NULL);
            timeDiff(&result, &endTime, &startTime);
            if (timeDiff(&_timeout, &_timeout, &result) <= 0){
                fail = true;
                _logger.addToLog("testing( "+ _tid + ") " + _url + " NOK -- Timeout on DNS lookup \n");
                break;
            }

            if (sock < 0)
                continue;
            break;
        }
        if (fail) {
            break;
        } 

        if (sock < 0) {
            _logger.addToLog("testing( "+ _tid + ") " + _url + " NOK -- Socket: " + strerror(errno) + "\n ");
            break;
        }
        
        if((arg = fcntl(sock, F_GETFL, NULL)) < 0 ){
            _logger.addToLog("testing( "+ _tid + ") " + _url + " NOK -- Get socket access mode: " + strerror(errno) + "\n ");
            break;         
        } 
        arg |= O_NONBLOCK; 

        if( fcntl(sock, F_SETFL, arg) < 0){
            _logger.addToLog("testing( "+ _tid + ") " + _url + " NOK -- Set socket access mode: " + strerror(errno) + "\n ");
        }
        break;
    }

    _sock = sock;
    _aInf = aInf;

    if (res != NULL){
       freeaddrinfo(res);
        res= NULL;        
    }
}
int HttpMonitor::checkHttp( std::vector<HttpMonitor>vmon) {

    struct addrinfo * res = NULL;
    int sock = -1;
    int status = -1;
    int ret, rc;
    struct addrinfo * aInf= NULL;
    long arg;
    bool fail = false;
    fd_set fset;
    std::string path ;
    std::string domain ;
    parseUrl(domain, path);
    std::size_t mcount = vmon.size();

    while ( true ){ 
        
        struct timeval startTime, endTime, result; 


////// Make connection

        fail = makeConnection(vmon);

        if (fail) {
            break;
        } 


//////// Send request
//
//        fail = sendRequest(vmon, domain, path, timeout);
//
//        if (fail) {
//            break;
//        } 
//        
//
//////// Get responce        
//        std::string str;
//        fail = getResponce(sock, str, timeout);
//
//        
//        if (fail) {
//            break;
//        } 
//        
//        struct timeval inittime=_timeout;        
//        std::stringstream ss;
//        ss << timeDiff(&result, &inittime, &timeout) / 1000;
//        std::string timeSpend =ss.str(); 
//        _logger.addToLog("testing ("+ _tid + ") " + _url + " OK " + timeSpend + "(ms) \n");
//       // std::cout<<"Responce: \n"<< str<<std::endl;
////////
//        status = 0;
//        break;
    }
    
    
    if (sock != -1){
        close(sock);
    }

    if (res != NULL){
       freeaddrinfo(res);
        res= NULL;        
    }
      
    return status;
}

long HttpMonitor::timeDiff( struct timeval* result, struct timeval* end, struct timeval * start) {

  long first =start->tv_sec*1000000 + start->tv_usec; 
  long last =end->tv_sec*1000000 + end->tv_usec; 
  long diff = last -first;
  if (diff < 0)
      return diff;
  result->tv_sec = diff / 1000000;
  result->tv_usec = diff % 1000000;
  return diff;
      
}

int HttpMonitor::headerReceived( std::string request) {
    int i = 0;
    while ( i< request.size() - 4) {
        if ( request[i] == '\r' && request[i + 1] == '\n' && request[i + 2] == '\r' && request[i + 3] == '\n'){
            return i + 3;
            }    
        if ( request[i] == '\n' && request[i + 1] == '\n'){
            return i + 1;
            }    
            i++;
    }
    return 0;
}
int HttpMonitor::getContentLength( std::string request) {
    std::regex rgx("Content-Length: (.*?)\r\n");
    std::smatch match;
    if(regex_search(request, match, rgx))
        return atoi(match[1].str().c_str());
    return -1;    
}

void HttpMonitor::parseUrl(std::string& host, std::string& path) {

    const char * url =_url.c_str();
    std::string x,protocol,port,query;
    int offset = 0;
    size_t pos1,pos2,pos3,pos4;
    x = trim(_url);
    offset = offset==0 && x.compare(0, 8, "https://")==0 ? 8 : offset;
    offset = offset==0 && x.compare(0, 7, "http://" )==0 ? 7 : offset;
    pos1 = x.find_first_of('/', offset+1 );
    path = pos1==std::string::npos ? "" : x.substr(pos1);
    host = std::string( x.begin()+offset, pos1 != std::string::npos ? x.begin()+pos1 : x.end() );
    path = (pos2 = path.find("#"))!=std::string::npos ? path.substr(0,pos2) : path;
    path = path.empty() ? "/" : path; 
    port = (pos3 = host.find(":"))!=std::string::npos ? host.substr(pos3+1) : "";
    host = host.substr(0, pos3!=std::string::npos ? pos3 : host.length());
    protocol = offset > 0 ? x.substr(0,offset-3) : "";
    query = (pos4 = path.find("?"))!=std::string::npos ? path.substr(pos4+1) : "";
    path = pos4!=std::string::npos ? path.substr(0,pos4).c_str() : path.c_str();
}

std::string HttpMonitor::trim(const std::string& str) {

    size_t start = str.find_first_not_of(" \n\r\t");
    size_t until = str.find_last_not_of(" \n\r\t");
    std::string::const_iterator i = start==std::string::npos ? str.begin() : str.begin() + start;
    std::string::const_iterator x = until==std::string::npos ? str.end()   : str.begin() + until+1;
    return std::string(i,x);

}

bool HttpMonitor::getResponce(int sock, std::string& str, struct timeval& timeout) {
        std::size_t buff_len = 100;
        char * buf = new char[buff_len];
        int rc=0;
        int ret=0;
        int cLength = 0;
        int cCounter = 0;
        fd_set fset;
        struct timeval startTime, endTime, result; 
        bool fail = false;

        while ( true ){
            FD_ZERO(&fset); 
            FD_SET(sock, &fset); 
        
            ret = select(sock+1, &fset, NULL, NULL, &timeout); 
            if(ret == 0){
                fail = true;
                _logger.addToLog("testing( "+ _tid + ") " + _url + " NOK -- Timeout on responce\n ");
                break;
            }

            if(ret < 0 && errno != EINTR){
                fail = true;
            _logger.addToLog("testing( "+ _tid + ") " + _url + " NOK -- Select for reading: " + strerror(errno) + "\n ");
                break;
            }

            gettimeofday(&startTime, NULL);

            rc = read(sock, buf, buff_len );

            gettimeofday(&endTime, NULL);
            timeDiff(&result, &endTime, &startTime);
            if (timeDiff(&timeout, &timeout, &result) <= 0){
                fail = true;
                _logger.addToLog("testing( "+ _tid + ") " + _url + " NOK -- Timeout on responce\n ");
                break;
            }

            if(rc == 0){
                break;
            }

            if(rc < 0){
                if(errno == EAGAIN || errno == EINTR)
                    continue;
                fail = true;
            _logger.addToLog("testing( "+ _tid + ") " + _url + " NOK -- Responce: " + strerror(errno) + "\n ");
                break;
            }

            str.append(buf, rc);

            if (cLength == 0 && headerReceived(str) != 0){
                cLength = getContentLength(str);
                rc = 0;
            }

            if(cLength > 0){
                cCounter+=rc; 
                if(cCounter >= cLength){
                    break;
                }
            }
        }
        if (buf != NULL)
            delete[] buf;

        return fail;
}


bool HttpMonitor::sendRequest(int sock,const  std::string& domain, const std::string& path, struct timeval& timeout) {

        int rc=0;
        int ret=0;
        fd_set fset;
        struct timeval startTime, endTime, result; 
        bool fail = false;

	    std::string request = "GET " + path + " HTTP/1.1\r\nHost: " + domain + "\r\nConnection: close\r\n\r\n";

        while (true) {
            FD_ZERO(&fset); 
            FD_SET(sock, &fset); 
        
            gettimeofday(&startTime, NULL);

            ret = select(sock+1, NULL, &fset, NULL, &timeout); 

            gettimeofday(&endTime, NULL);
            timeDiff(&result, &endTime, &startTime);
            if (timeDiff(&timeout, &timeout, &result) <= 0){
                fail = true;
                _logger.addToLog("testing( "+ _tid + ") " + _url + " NOK -- Timeout on sending request\n ");
                break;
            }

            if(ret == 0){
                fail = true;
                _logger.addToLog("testing( "+ _tid + ") " + _url + " NOK -- Timeout on sending request\n ");
                break;
            }

            if(ret < 0 && errno != EINTR){
                fail = true;
            _logger.addToLog("testing( "+ _tid + ") " + _url + " NOK -- Select on sending: " + strerror(errno) + "\n ");
                break;
            } else if (errno == EAGAIN)
                continue;

            gettimeofday(&startTime, NULL);
            rc = send(sock, request.c_str(),  strlen(request.c_str()), 0);

            gettimeofday(&endTime, NULL);
            timeDiff(&result, &endTime, &startTime);
            if (timeDiff(&timeout, &timeout, &result) <= 0){
                fail = true;
                _logger.addToLog("testing( "+ _tid + ") " + _url + " NOK -- Timeout on sending request\n ");
                break;
            }

            if(rc < 0){
                if(errno = EINTR)
                    continue;
            _logger.addToLog("testing( "+ _tid + ") " + _url + " NOK -- Send: " + strerror(errno) + "\n ");
                fail = true;
                break;
            }     
            request.erase(request.begin(), request.begin() + rc);
            if (request.empty())
                break;
        }
        return fail;
}

bool HttpMonitor::makeConnection(std::vector<HttpMonitor> vmon) {

        int rc=0;
        int ret=0;
        fd_set fset;
        FD_ZERO(&fset); 
        struct timeval startTime, endTime, result; 
        bool fail = false;

        gettimeofday(&startTime, NULL);
        int i=0;
        while (i !=vmon.size()){
    	while ((rc = connect(vmon[i]._sock, vmon[i]._aInf->ai_addr, vmon[i]._aInf->ai_addrlen)) < 0 && errno == EINTR ){}; 
        if (rc < 0 && errno != EINPROGRESS ) {
            _logger.addToLog("testing( "+ vmon[i]._tid + ") " + vmon[i]._url + " NOK -- Connect: " + strerror(errno) + "\n ");
            fail = true;
        } 
        FD_SET(vmon[i]._sock, &fset); 
        ++i;
        }

        if (rc < 0 && errno == EINPROGRESS) {
            while ( true ){
                gettimeofday(&endTime, NULL);
                timeDiff(&result, &endTime, &startTime);
                if (timeDiff(&vmon[i]._timeout, &vmon[i]._timeout, &result) <= 0){
                    fail = true;
                    _logger.addToLog("testing( "+ _tid + ") " + _url + " NOK -- Timeout on connection \n ");
                    break;
                }

                    ret = select(maxFd(vmon) + 1, NULL, &fset, NULL, NULL);
                    gettimeofday(&startTime, NULL);

                    if(ret > 0){
                        for(int j=0; j< vmon.size(); ++i){
                             int valopt;
                             socklen_t lon = sizeof(int); 
                             if (FD_ISSET(vmon[i]._sock, &fset)){
                                if (getsockopt(vmon[i]._sock, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0){
                                    fail = true;
                                    _logger.addToLog("testing( "+ vmon[i]._tid + ") " +vmon[i]._url + " NOK -- Getsocksetoption after connection: " + strerror(errno) + "\n ");
                                } 
                                if(valopt){
                                    fail = true;
                                    _logger.addToLog("testing( "+vmon[i]._tid + ") " +vmon[i]._url + " NOK --  Error in connection" + strerror(valopt) + " \n ");
                                }
                             }
                        }   
                       break;
                    }

                   // if(ret == 0){
                   //     fail = true;
                   //     _logger.addToLog("testing( "+ _tid + ") " + _url + " NOK -- Timeout on connection \n ");
                   //     break;
                   // }

                   // if(ret < 0 && errno == EINTR)  
                   //     continue;
                   // _logger.addToLog("testing( "+ _tid + ") " + _url + " NOK -- Select for connection: " + strerror(errno) + "\n ");
                   // fail = true;
            }
        }

        gettimeofday(&endTime, NULL);
        //timeDiff(&result, &endTime, &startTime);
        //if (timeDiff(&timeout, &timeout, &result) <= 0 && !fail){
        //    fail = true;
        //    _logger.addToLog("testing( "+ _tid + ") " + _url + " NOK -- Timeout on connection \n ");
        //}
        return fail;
}

int HttpMonitor::maxFd(std::vector<HttpMonitor> vmon) const {
    int max = 0;
    for (std::vector<HttpMonitor>::iterator it = vmon.begin() ; it != vmon.end(); ++it)
        max = max < (*it)._sock ? (*it)._sock : max;  

   return max;
    
 }
