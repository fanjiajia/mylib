#include "threadpool.h"

CThreadPool::CThreadPool()
{
}

CThreadPool::~CThreadPool()
{
    for (auto &item : m_list) item->join();
}

bool CThreadPool::threadpoolStart(int nThreadNum, THREAD_FUNC func, void *param)
{
    if (nThreadNum < 0) return false;

    for (int i = 0; i < nThreadNum; i++)
    {
        std::shared_ptr<std::thread> m_thread;
        m_thread = std::make_shared<std::thread>(func, param);
        m_list.push_back(m_thread);
    }

    return true;
}
