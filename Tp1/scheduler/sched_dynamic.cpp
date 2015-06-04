#include "sched_dynamic.h"
#include <iostream>

using namespace std;

SchedDynamic::SchedDynamic(vector<int> argn): ciclo(0), deadlines(argn[0], 0) {
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
	if ((m == EXIT) || (m == BLOCK) || current_pid(cpu) == IDLE_TASK) {
		if (q.empty()) return IDLE_TASK;
		int pid = q.top().pid;
		deadlines[cpu] = q.top().deadline;
		q.pop();
		return pid;
	}
	// hay una tarea con mayor prioridad?
	if (deadlines[cpu] > q.top().deadline) {
		task t({current_pid(cpu), deadlines[cpu]});
		int pid = q.top().pid;
		deadlines[cpu] = q.top().deadline;
		q.pop();
		q.push(t);
		return pid;
	}
	return current_pid(cpu);
}
