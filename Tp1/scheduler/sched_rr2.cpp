#include <vector>
#include <queue>
#include "sched_rr2.h"
#include "basesched.h"
#include <iostream>

using namespace std;

SchedRR2::SchedRR2(vector<int> argn): quantum(++(++argn.begin()), argn.end()), quantum_status(argn[0], 0), qs(argn[0]) {
	// Round robin recibe la cantidad de cores y sus cpu_quantum por parámetro
}

SchedRR2::~SchedRR2() {

}


void SchedRR2::load(int pid) {
	int cpu=0;
	for (int i = 1; i < qs.size(); ++i)
		if (qs[i].size() < qs[cpu].size()) cpu=i;
	qs[cpu].push(pid);
	proc_cpu[pid] = cpu;
}

void SchedRR2::unblock(int pid) {
	qs[proc_cpu[pid]].push(pid);
}

int SchedRR2::tick(int cpu, const enum Motivo m) {
	if ((m == EXIT) || (m == BLOCK)) {		
	// Si el pid actual terminó, sigue el próximo.
		quantum_status[cpu] = 0;
		if (qs[cpu].empty()) {
			return IDLE_TASK;
		} else {
			int sig = qs[cpu].front(); qs[cpu].pop();
			return sig;
		}
	} else {
		/*if(current_pid(cpu) == 1) {
			cout << endl << endl << "Soy task uno , quantum_status:" << quantum_status[cpu] << endl << endl;
		}*/
		quantum_status[cpu]++;
		if (current_pid(cpu) == IDLE_TASK && !qs[cpu].empty()) {
			int sig = qs[cpu].front(); qs[cpu].pop();
			quantum_status[cpu] = 0;
			return sig;
		} else if (quantum_status[cpu] == quantum[cpu] && current_pid(cpu) != IDLE_TASK) {
			quantum_status[cpu] = 0;
			qs[cpu].push(current_pid(cpu));
			int sig = qs[cpu].front(); qs[cpu].pop();
			return sig;
		} else {
			return current_pid(cpu);
			}
	}

}

int SchedRR2::next(int cpu) {
}
