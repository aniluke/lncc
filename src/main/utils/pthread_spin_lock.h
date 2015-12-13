#ifndef _PTHREAD_SPIN_LOCK_H_
#define _PTHREAD_SPIN_LOCK_H_

#include <base_lock.h>
#include <pthread.h>

namespace lncc {
    class PthreadSpinLock: public BaseLock {
    public:
        PthreadSpinLock();
        virtual ~PthreadSpinLock();
        
        virtual void lock();
        virtual void unlock();

    private:
        pthread_spinlock_t m_lock;
    };
}
#endif
