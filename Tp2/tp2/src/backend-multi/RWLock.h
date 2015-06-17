#ifndef RWLock_h
#define RWLock_h
#include <iostream>

class RWLock {
    public:
        RWLock();
        void rlock();
        void wlock();
        void runlock();
        void wunlock();

    private:
        //pthread_rwlock_t rwlock;
        pthread_cond_t cond;
       // pthread_cond_t read;
        pthread_mutex_t mut;
        pthread_mutex_t hayWrites;
        int reading;
        int writing;
};

#endif
