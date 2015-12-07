#ifndef _NULL_LOCK_H_
#define _NULL_LOCK_H_

#include <base_lock.h>

namespace lncc {
    class NullLock: public BaseLock {
    public:
        NullLock() {}
        virtual ~NullLock() {};

        virtual void lock() {}
        virtual void unlock() {}

    };
}

#endif
