#ifndef _PTHREAD_LOCK_H_
#define _PTHREAD_LOCK_H_

#include <base_lock.h>
#include <pthread.h>

namespace lncc {
    class PthreadLock: public BaseLock {
    public:
        PthreadLock();
        virtual ~PthreadLock();
        
        virtual void lock();
        virtual void unlock();

    private:
        pthread_mutex_t m_mutex;
    };
}
#endif
