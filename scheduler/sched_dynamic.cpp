#include "sched_dynamic.h"
#include <iostream>

using namespace std;

SchedDynamic::SchedDynamic(vector<int> argn): ciclo(0) {
}

SchedDynamic::~SchedDynamic() {
}

void SchedDynamic::initialize() {
}

void SchedDynamic::load(int pid) {
	task t({pid, ciclo + period(pid)});
	q.push(t);
}

void SchedDynamic::unblock(int pid) {
	task t({pid, ciclo + period(pid)});
	q.push(t);
}

int SchedDynamic::tick(int cpu, const enum Motivo m) {
	ciclo++;
	if ((m == EXIT) || (m == BLOCK)) {
		if (q.empty()) return IDLE_TASK;
		task t = q.top();
		q.pop();
		return t.pid;
	}
	if (current_pid(cpu) == IDLE_TASK && !q.empty()) {
		task t = q.top();
		q.pop();
		return t.pid;
	}
	return current_pid(cpu);
}
