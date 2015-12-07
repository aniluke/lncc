#include <lock_factory.h>
#include "pthread_lock.h"
#include "null_lock.h"

using namespace lncc;

int LockFactory::m_default_lock_type = E_DEFAULT_LOCK;

void LockFactory::set_default_lock(int lock_type) {
    m_default_lock_type = lock_type;
}

BaseLock* LockFactory::create_lock(int lock_type) {
    if (lock_type == E_PTHREAD_LOCK) {
        return new PthreadLock();
    }

    if (lock_type == E_NULL_LOCK) {
        return new NullLock();
    }

    // default
    if (m_default_lock_type != E_DEFAULT_LOCK) {
        return create_lock(m_default_lock_type);
    }

    return create_lock(E_PTHREAD_LOCK);
}
