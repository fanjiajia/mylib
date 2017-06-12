#ifndef _THREADPOOL_H__
#define _THREADPOOL_H__
#include <thread>
#include <memory>
#include <list>

typedef void* (*THREAD_FUNC)(void*);

class CThreadPool
{
public:
    CThreadPool();
    ~CThreadPool();

public:
    bool threadpoolStart(int nThreadNum, THREAD_FUNC func, void *param);
private:
    std::list<std::shared_ptr<std::thread>> m_list;
};


#endif

