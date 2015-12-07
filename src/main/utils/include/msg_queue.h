#ifndef _MSG_QUEUE_H_
#define _MSG_QUEUE_H_

#include <list>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <lock_factory.h>

namespace lncc {
    template<class T> class MsgQueue {
    public:
        std::list<T> m_queue;
        size_t m_curr_size;
        BaseLock* m_lock;
        sem_t m_consumer_sem;
        sem_t m_producer_sem;
        int m_max_size;
    public:
        MsgQueue(int max_size) {
            m_lock = LockFactory::create_lock(0);
            sem_init(&m_consumer_sem, 0, 0);
            m_max_size = max_size;
            m_curr_size = 0;
            if (max_size > 0)
                sem_init(&m_producer_sem, 0, max_size);
        }

        void push(const T& value) {
            if (m_max_size > 0) {
                sem_wait(&m_producer_sem);
            }
 
            m_lock->lock();
            m_queue.push_back(value);
            m_curr_size ++;
            sem_post(&m_consumer_sem);
            m_lock->unlock();
        }

        T take() {
            sem_wait(&m_consumer_sem);
            m_lock->lock();
            T value = m_queue.front();
            m_queue.pop_front();
            m_curr_size --;
            if (m_max_size > 0)
                sem_post(&m_producer_sem);
            m_lock->unlock();
            return value;
        }

        bool try_take(T& out) {
            int ret = sem_trywait(&m_consumer_sem);
            if (ret == -1 && errno == EAGAIN)
                return false;

            m_lock->lock();
            out = m_queue.front();
            m_queue.pop_front();
            m_curr_size --;
            if (m_max_size > 0)
                sem_post(&m_producer_sem);
            m_lock->unlock();
            return true;
        }

        size_t size() {
            return m_curr_size;
        }
    };
}

#endif
