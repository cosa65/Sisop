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

/*void RWLock :: rlock() {	

	pthread_mutex_lock(&hayWrites);
	pthread_mutex_unlock(&hayWrites);

	pthread_mutex_lock(&mutreading);
	reading++;
	waiting++;
	if(reading == 1){				//Si soy el primero, espero a que se acabo el write y luego dejo entrar a todos los reads
		pthread_mutex_unlock(&mutreading);
		pthread_mutex_lock(&mutreading);
		pthread_mutex_lock(&mutreading);
		waiting--;
		pthread_mutex_unlock(&mutreading);
		pthread_cond_broadcast(&cond);
	} else {						//Si no soy el primero, espero a que el primero entre y me avise que puedo entrar, o paso si ya puedo pasar
		if(waiting == 1){			//Si soy el primero esperando, pero no soy el primero, significa que puedo pasar
			waiting--;
			pthread_mutex_unlock(&mutreading);
		} else {
					// Si hay otros esperando y no soy el primero, significa que el primero no paso
			while(writing == 0){
			pthread_cond_wait(&cond, &mutreading);
			}
			waiting--;
			pthread_mutex_unlock(&mutreading);
		}
	}

}
*/

/* 
void RWLock :: runlock() {

	pthread_mutex_lock(&mutreading);
	reading--;
	if(reading == 0){				//Si soy el ultimo, dejo que pasen los writes
		pthread_mutex_unlock(&mutreading);
	}
	pthread_mutex_unlock(&mutreading);
}

*/
