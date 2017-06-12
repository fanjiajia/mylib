#ifndef __PERSISTENT_CONNECT_POLL_HEADER_H_
#define __PERSISTENT_CONNECT_POLL_HEADER_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <memory>
#include <list>
#include <pthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <event2/event.h>
#include <netinet/tcp.h>

class CPersistentConnect
{
public:
    CPersistentConnect(std::string _ip, int _port);
    ~CPersistentConnect();

public:
    bool init_connect();
    bool setkeepalive();

public:
    std::string m_ip_;
    int m_port_;

    evutil_socket_t m_fd_;
};

typedef std::shared_ptr<CPersistentConnect> PersistentConnectPtr;


class CPersistentConnectPoll
{
public:
    CPersistentConnectPoll(std::string host, int port);
    ~CPersistentConnectPoll();

    PersistentConnectPtr GetConnection();
    void ReleaseConnection(PersistentConnectPtr conn);

public:
    std::string _ip;
    int _port;

    std::list<PersistentConnectPtr> client_list_;
    pthread_mutex_t *m_mutexlock;
};


typedef std::shared_ptr<CPersistentConnectPoll> PersistentConnectPollPtr;
#endif
