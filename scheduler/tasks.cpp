#include "tasks.h"
#include <stdlib.h>
#include <iostream>
#include <algorithm>
using namespace std;

void TaskCPU(int pid, vector<int> params) { // params: n
	uso_CPU(pid, params[0]); // Uso el CPU n milisegundos.
}

void TaskIO(int pid, vector<int> params) { // params: ms_pid, ms_io,
	uso_CPU(pid, params[0]); // Uso el CPU ms_pid milisegundos.
	uso_IO(pid, params[1]); // Uso IO ms_io milisegundos.
}

void TaskAlterno(int pid, vector<int> params) { // params: ms_pid, ms_io, ms_pid, ...
	for(int i = 0; i < (int)params.size(); i++) {
		if (i % 2 == 0) uso_CPU(pid, params[i]);
		else uso_IO(pid, params[i]);
	}
}

void TaskConBloqueo(int pid, vector<int> params) {
	int tiempo = params[0];
	int inicioBloq = params[1];
	int finBloq = params[2];

	if(inicioBloq > tiempo){
		uso_CPU(pid, tiempo);
	}else{
		uso_CPU(pid,inicioBloq-2);
		uso_IO(pid,finBloq-inicioBloq+1);
		uso_CPU(pid,tiempo-finBloq);
	}
}

void TaskConsola(int pid, vector<int> params) {
	int n = params[0];
	int bmin = params[1];
	int bmax = params[2];

	for (int i = 0; i < n; ++i)
	{
		int duracion = rand() % (bmax-bmin+1) + bmin;
		uso_IO(pid,duracion);
	}
}

void TaskBatch(int pid, vector<int> params) {
    int cpu_time = params[0];
    int cant_bloq = params[1];
    vector<int> bloq_moment(cant_bloq);
    for (int i = 0; i < cant_bloq; ++i){
        bloq_moment[i] = rand() % cpu_time;
    }
    std::sort(bloq_moment.begin(), bloq_moment.end());
    int cpu_usado = 0;
    for (int i = 0; i< cant_bloq; ++i){
        uso_CPU(pid, bloq_moment[i] - cpu_usado);
        uso_IO(pid, 1);
        cpu_usado += bloq_moment[i] - cpu_usado + 1;
    }
    uso_CPU(pid, cpu_time - cpu_usado);

}
void tasks_init(void) {
	/* Todos los tipos de tareas se deben registrar acá para poder ser usadas.
	 * El segundo parámetro indica la cantidad de parámetros que recibe la tarea
	 * como un vector de enteros, o -1 para una cantidad de parámetros variable. */
	register_task(TaskCPU, 1);
	register_task(TaskIO, 2);
	register_task(TaskAlterno, -1);
	register_task(TaskConBloqueo, 3);
	register_task(TaskConsola, 3);
    register_task(TaskBatch, 2);
}
