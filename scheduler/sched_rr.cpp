#include <vector>
#include <queue>
#include "sched_rr.h"
#include "basesched.h"
#include <iostream>
#include <iterator>

using namespace std;

SchedRR::SchedRR(vector<int> argn): quantum(++argn.begin(), argn.end()), quantum_status(argn[0], 0) {
	// Round robin recibe la cantidad de cores y sus cpu_quantum por parámetro
	cout << "tiene tamanio " << quantum.size() << endl;
}

SchedRR::~SchedRR() {

}


void SchedRR::load(int pid) {
	q.push(pid);
}

void SchedRR::unblock(int pid) {
	q.push(pid);
}

int SchedRR::tick(int cpu, const enum Motivo m) {
	if ((m == EXIT) || (m == BLOCK)) {		
	// Si el pid actual terminó, sigue el próximo.
		if (q.empty()) return IDLE_TASK;
		else {
			int sig = q.front(); q.pop();
			return sig;
		}
	} else {
		quantum_status[cpu]++;
		if (current_pid(cpu) == IDLE_TASK && !q.empty()) {
			int sig = q.front(); q.pop();
			quantum_status[cpu] = 0;
			return sig;
		} else if (quantum_status[cpu] == quantum[cpu] && current_pid(cpu) != IDLE_TASK) {
			quantum_status[cpu] = 0;
			q.push(current_pid(cpu));
			int sig = q.front(); q.pop();
			return sig;
		} else {
			return current_pid(cpu);
		}
	}
}

// int SchedRR::next(int cpu) {
// }
