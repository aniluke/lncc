#include "pthread_lock.h"

using namespace lncc;

PthreadLock::PthreadLock():m_mutex(PTHREAD_MUTEX_INITIALIZER) {
    pthread_mutex_init(&m_mutex, NULL);
}
PthreadLock::~PthreadLock() {
    pthread_mutex_destroy(&m_mutex);
}

void PthreadLock::lock() {
    pthread_mutex_lock(&m_mutex);
}

void PthreadLock::unlock() {
    pthread_mutex_unlock(&m_mutex);
}
