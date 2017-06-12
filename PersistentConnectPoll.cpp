#include "PersistentConnectPoll.h"

CPersistentConnect::CPersistentConnect(std::string _ip, int _port)
{
    m_ip_ = _ip;
    m_port_ = _port;
}

CPersistentConnect::~CPersistentConnect()
{
    evutil_closesocket(m_fd_);
}

bool CPersistentConnect::setkeepalive()
{
    int keepAlive = 1;
    int keepIdle = 5;
    int keepInterval = 5;
    int keepCount = 3;
    if (setsockopt(m_fd_, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive)) == -1) return false;
    if (setsockopt(m_fd_, SOL_TCP, TCP_KEEPIDLE, (void *)&keepIdle, sizeof(keepIdle)) == -1) return false;
    if (setsockopt(m_fd_, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval)) == -1) return false;
    if (setsockopt(m_fd_, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount)) == -1) return false;
    return true;
}

bool CPersistentConnect::init_connect()
{
    struct sockaddr_in sin;
    memset(&sin, 0x00, sizeof(sin));

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(m_ip_.c_str());
    sin.sin_port = htons(m_port_);

    if ((m_fd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) return false;

    struct sockaddr_in b_sin;
    memset(&b_sin, 0x00, sizeof(b_sin));
    b_sin.sin_family = AF_INET;
    b_sin.sin_port = htons(0);
    b_sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    bind(m_fd_, (struct sockaddr*)&b_sin, sizeof(b_sin));

    evutil_make_socket_nonblocking(m_fd_);
    evutil_make_listen_socket_reuseable(m_fd_);

    setkeepalive();

    if (connect(m_fd_, (struct sockaddr*)&sin, sizeof(sin)) < 0)
    {
        if (errno != EINPROGRESS && errno != EINTR)
        {
            close(m_fd_);
            return false;
        }
    }

    return true;
}

CPersistentConnectPoll::CPersistentConnectPoll(std::string host, int port)
{
    _ip = host;
    _port = port;

    m_mutexlock = new pthread_mutex_t;
    pthread_mutex_init(m_mutexlock, NULL);
}

CPersistentConnectPoll::~CPersistentConnectPoll()
{
    pthread_mutex_destroy(m_mutexlock);
    delete m_mutexlock;
}

PersistentConnectPtr CPersistentConnectPoll::GetConnection()
{
    PersistentConnectPtr res = NULL;

    pthread_mutex_lock(m_mutexlock);
    if (client_list_.empty())
    {
        PersistentConnectPtr conn(new CPersistentConnect(_ip, _port));
        if (conn->init_connect()) res = conn;
    }
    else
    {
        res = client_list_.front();
        client_list_.pop_front();
    }
    pthread_mutex_unlock(m_mutexlock);
    return res;
}

void CPersistentConnectPoll::ReleaseConnection(PersistentConnectPtr conn)
{
    pthread_mutex_lock(m_mutexlock);
    client_list_.push_back(conn);
    pthread_mutex_unlock(m_mutexlock);
}


#if 0
int main()
{
    PersistentConnectPollPtr clientpoll_ptr(new CPersistentConnectPoll("58.221.38.183", 80));

    PersistentConnectPtr m_ptr = clientpoll_ptr->GetConnection();
    printf("size: %d\n", clientpoll_ptr->client_list_.size());
    clientpoll_ptr->ReleaseConnection(m_ptr);
    printf("size: %d\n", clientpoll_ptr->client_list_.size());

    return 0;
}

#endif
