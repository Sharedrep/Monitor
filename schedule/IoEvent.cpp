/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "IoEvent.h"

IoEvent::IoEvent() : m_ownsFDs(true)
{
    if (pipe(m_pipe) < 0)
       std::cout<<"Failed to create pipe: "<<strerror(errno)<<std::endl; 
    
    if (fcntl(m_pipe[0], F_SETFL, O_NONBLOCK) < 0)
       std::cout<<"Failed to set pipe non-blocking mode: "<<strerror(errno)<<std::endl; 
}

IoEvent::IoEvent(int fd) :m_ownsFDs(false) 
{
    m_pipe[0] = fd;
    m_pipe[1] = -1;
}

IoEvent::IoEvent(const IoEvent &other) 
{
    m_pipe[0] = other.m_pipe[0];
    m_pipe[1] = other.m_pipe[1];
    m_ownsFDs = false;
}

IoEvent::~IoEvent() 
{
    if (m_pipe[0] >= 0)
    {
        if (m_ownsFDs)
            close(m_pipe[0]);
        m_pipe[0] = -1;
    }

    if (m_pipe[1] >= 0)
    {
        if (m_ownsFDs)
            close(m_pipe[1]);
        m_pipe[1] = -1;
    }
}

void IoEvent::set() 
{
    if (m_ownsFDs)
        write(m_pipe[1], "x", 1);
}

void IoEvent::reset()
{
    if (m_ownsFDs)
    {
        uint8_t buf;

        while (read(m_pipe[0], &buf, 1) == 1);
    }
}
