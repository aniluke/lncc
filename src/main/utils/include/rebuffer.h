#ifndef _RE_BUFFER_H_
#define _RE_BUFFER_H_
#include <common.h>
#include <stdio.h>


namespace lncc {
    // Re-used Buffer
    template<class T>
    class ReBuffer {
    public:
        ReBuffer(uint64_t max_size_reused):m_max_size_reused(max_size_reused),
                                           m_buf(NULL),
                                           m_size(0),
                                           m_max_size(0) {
        }
        
        ~ReBuffer() {
            if (m_buf) {
                delete[] m_buf;
            }
        }

        void alloc(uint64_t size) {
            if (size > m_max_size) {
                if (m_buf) {
                    delete[] m_buf;
                }

                m_max_size = size;
                m_buf = new T[m_max_size];
            }
            m_size = size; // allow 0
            
        }

        void clear() {
            if (m_max_size_reused > 0 && m_max_size > m_max_size_reused) {
                delete[] m_buf;
                m_buf = NULL;
                m_max_size = 0;
            }

            m_size = 0;
        }
        
        T* data() { return m_buf;}
        const T* data() const { return m_buf; }
        uint64_t size() { return m_size; }
        uint64_t max_size() { return m_max_size; }

    private:
        uint64_t m_max_size_reused;
        T* m_buf;
        uint64_t m_size;
        uint64_t m_max_size;

    };
}


#endif
