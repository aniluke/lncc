#ifndef _BASE_LOCK_H_
#define _BASE_LOCK_H_

namespace lncc {
    class BaseLock {
    public:
        virtual ~BaseLock() {};
        
        virtual void lock() = 0;
        virtual void unlock() = 0;

    };
}
#endif
