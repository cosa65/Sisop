#include "RWLock.h"

RWLock :: RWLock()/* : rwlock()*/{
	pthread_mutex_init(&mut, NULL);
	pthread_mutex_init(&mutreading, NULL);
	pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&hayWrites, NULL);
	reading = 0;


}

void RWLock :: rlock() {	

	pthread_mutex_lock(&hayWrites);
	pthread_mutex_unlock(&hayWrites);

	pthread_mutex_lock(&mutreading);
	reading++;
	if(reading == 1){				//Si soy el primero, espero a que se acabo el write y luego dejo entrar a todos los reads
		pthread_mutex_unlock(&mutreading);
		pthread_mutex_lock(&mut);
		pthread_cond_broadcast(&cond);
	} else {						//Si no soy el primero, espero a que el primero entre y me avise que puedo entrar
		pthread_cond_wait(&cond, &mutreading);
		pthread_mutex_unlock(&mutreading);
	}

}

void RWLock :: wlock() {

	pthread_mutex_lock(&hayWrites);
	pthread_mutex_lock(&mut);

}

void RWLock :: runlock() {

	pthread_mutex_lock(&mutreading);
	reading--;
	if(reading == 0){				//Si soy el ultimo, dejo que pasen los writes
		pthread_mutex_unlock(&mut);
	}
	pthread_mutex_unlock(&mutreading);
}

void RWLock :: wunlock() {
	pthread_mutex_unlock(&hayWrites);
	pthread_mutex_unlock(&mut);

}
