#ifndef _LOCK_FACTORY_H_
#define _LOCK_FACTORY_H_

#include <base_lock.h>

namespace lncc {
    class LockFactory {
    public:
        enum {
            E_DEFAULT_LOCK,
            E_PTHREAD_LOCK,
            E_NULL_LOCK
        };

    public:
        static void set_default_lock(int lock_type);
        static BaseLock* create_lock(int lock_type);

    private:
        static int m_default_lock_type;

    };
}
#endif
