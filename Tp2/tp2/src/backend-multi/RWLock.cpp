#include "RWLock.h"

RWLock :: RWLock()/* : rwlock()*/{
	pthread_mutex_init(&mut, NULL);
	pthread_cond_init(&cond, NULL);	
	pthread_mutex_init(&hayWrites, NULL);				//Evite inanicion y controla interaccion entre writes (lo toma un write y todos se encolan detras de el)
	writing = 0;
	reading = 0;

}

void RWLock :: rlock() {	

	pthread_mutex_lock(&hayWrites);
	pthread_mutex_unlock(&hayWrites);

	pthread_mutex_lock(&mut);
	
	while(writing != 0){		
		pthread_cond_wait(&cond, &mut);
	}

	reading++;
	pthread_mutex_unlock(&mut);

}

void RWLock :: wlock() {

	pthread_mutex_lock(&hayWrites);	

	pthread_mutex_lock(&mut);
	
	while(reading > 0){
		pthread_cond_wait(&cond, &mut);
	}

	writing++;

}

void RWLock :: runlock() {

	pthread_mutex_lock(&mut);
	reading--;
	pthread_mutex_unlock(&mut);
	
	pthread_cond_signal(&cond);

}

void RWLock :: wunlock() {

	writing--;	
	pthread_mutex_unlock(&mut);

	pthread_mutex_unlock(&hayWrites);
	
	pthread_cond_broadcast(&cond);

} 