#include "sched_fixed.h"
#include <iostream>

using namespace std;

SchedFixed::SchedFixed(vector<int> argn) {
}

SchedFixed::~SchedFixed() {
}

void SchedFixed::initialize() {
}

void SchedFixed::load(int pid) {
	q.push( make_pair(period(pid), pid) );
}

void SchedFixed::unblock(int pid) {
//	q.push( make_pair(period(pid), pid) );
}

int SchedFixed::tick(int cpu, const enum Motivo m) {

	if(m==EXIT){
		if(q.empty()) return IDLE_TASK;
		else {
			int sig = q.top().second; q.pop();
			return sig;
		}	
	} else {
		// Siempre sigue el pid actual mientras no termine.
		if (current_pid(cpu) == IDLE_TASK && !q.empty()) {
			int sig = q.top().second; q.pop();
			return sig;
		} else {
			return current_pid(cpu);
		}
	}

}
