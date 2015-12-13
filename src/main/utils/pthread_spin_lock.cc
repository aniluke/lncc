#include "pthread_spin_lock.h"

using namespace lncc;

PthreadSpinLock::PthreadSpinLock() {
    pthread_spin_init(&m_lock, PTHREAD_PROCESS_PRIVATE);
}
PthreadSpinLock::~PthreadSpinLock() {
    pthread_spin_destroy(&m_lock);
}

void PthreadSpinLock::lock() {
    pthread_spin_lock(&m_lock);
}

void PthreadSpinLock::unlock() {
    pthread_spin_unlock(&m_lock);
}
