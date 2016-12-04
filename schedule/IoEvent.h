#include <iostream>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>

#ifndef IOEVENT_H
#define IOEVENT_H
class IoEvent {
public:
    IoEvent();             
    IoEvent(int fd);      

    IoEvent(const IoEvent &other);

    virtual ~IoEvent();
    void set();

    void reset();

    inline int fd() const 
    {
        return m_pipe[0];
    }

protected:

    int m_pipe[2];
    bool m_ownsFDs;

};

#endif /* IOEVENT_H */

