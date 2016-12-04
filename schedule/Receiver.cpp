/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "Receiver.h"

struct Responce {
    Responce () : cCounter(0), cLength(0){};
    std::string str;
    int cCounter;
    int cLength;
    };

Receiver::Receiver() 
{
}

Receiver::Receiver(const Receiver& orig) 
{
}

Receiver::~Receiver() 
{
}

bool Receiver::getResponce() 
{
        std::map<int, Responce> responce;
        std::size_t buff_len = 5;
        char * buf = new char[buff_len];
        int rc = 0;
        int ret = 0;
        int fd_count = 0;
        fd_set fset;
        bool fail = false;
        struct timeval startTime, endTime, result; 
        struct timeval timeout = {this->minTimeout(), 0};

        while ( true )
        {
        
            ret = select(_maxFd+1, &fset, NULL, NULL, &timeout); 

            if(ret == 0)
                break;

            if(ret < 0 && errno != EINTR)
            {
              //_logger.addToLog("NOK -- Select for reading: " + strerror(errno) + "\n ");
                break;
            }
        }    

        while (fd_count < testlist->size())
        {
            gettimeofday(&startTime, NULL);
            for(auto it = testlist->begin(); it != testlist->end(); ++it) 
            {

                if((*it).getSock() != -1 && FD_ISSET((*it).getSock(), &fset))
                {
                    gettimeofday(&startTime, NULL);

                    rc = read((*it).getSock(), buf, buff_len );

                    gettimeofday(&endTime, NULL);
                    timeDiff(&result, &endTime, &startTime);
                    if (timeDiff(&(*it)._timeout, &(*it)._timeout, &result) <= 0)
                    {
                        _logger.addToLog("testing( "+ (*it)._tid + ") " + (*it)._url + " NOK -- Timeout on responce\n ");
                        continue;
                    }

                    if(rc == 0)
                    {
                        ++fd_count;
                        FD_CLR((*it).getSock(), &fset);
                        std::cout<<"Response received from "<<(*it).getSock()<<std::endl;
                        struct timeval inittime=(*it)._timeout;        
                        std::stringstream ss;
                        ss << timeDiff(&result, &inittime, &timeout) / 1000;
                        std::string timeSpend =ss.str(); 
                        _logger.addToLog("testing ("+(*it)._tid + ") " + (*it)._url + " OK " + timeSpend + "(ms) \n");
                        continue;
                    }

                    if(rc < 0)
                    {
                        if(errno == EAGAIN || errno == EINTR)
                            continue;
                        _logger.addToLog("testing( "+ (*it)._tid + ") " + (*it)._url + " NOK -- Responce: " + strerror(errno) + "\n ");
                        ++fd_count;
                    }

                    responce[(*it).getSock()].str.append(buf, rc);

                    if (responce[(*it).getSock()].cLength == 0 && (*it).headerReceived(responce[(*it).getSock()].str) != 0)
                    {
                        responce[(*it).getSock()].cLength = (*it).getContentLength(responce[(*it).getSock()].str);
                        rc = 0;
                    }

                    if(responce[(*it).getSock()].cLength > 0)
                    {
                        responce[(*it).getSock()].cCounter+=rc; 
                        if(responce[(*it).getSock()].cCounter >= responce[(*it).getSock()].cLength)
                        {
                            ++fd_count;
                            FD_CLR((*it).getSock(), &fset);
                            std::cout<<"Responce recieved from "<<(*it).getSock()<<std::endl;
                            continue;
                        }
                    }
                }
            }
        }

        if (buf != NULL)
            delete[] buf;

        return fail;
}

void Receiver::AddMonitor(HttpMonitor* pmon) 
{
   testlist->emplace_back(*pmon);
}

void Receiver::maxFd(int fd) 
{
  _maxFd = (fd > _maxFd) ? fd : _maxFd; 
}

long Receiver::minTimeout() const 
{
    long min = 0;
    long timeoutS;
    for (std::vector<HttpMonitor>::const_iterator it = testlist->begin() ; it != testlist->end(); ++it) 
    {

        timeoutS =(*it)._timeout.tv_sec + (*it)._timeout.tv_usec/1000;
        min = min < timeoutS ? timeoutS : min;  
    }
    return min;
}

long Receiver::timeDiff( struct timeval* result, struct timeval* end, struct timeval * start) 
{

  long first =start->tv_sec*1000000 + start->tv_usec; 
  long last =end->tv_sec*1000000 + end->tv_usec; 
  long diff = last -first;
  if (diff < 0)
      return diff;
  result->tv_sec = diff / 1000000;
  result->tv_usec = diff % 1000000;
  return diff;
      
}
