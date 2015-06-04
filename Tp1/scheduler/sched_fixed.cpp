#include "sched_fixed.h"
#include <iostream>

using namespace std;

SchedFixed::SchedFixed(vector<int> argn): periods(argn[0],0) {
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
		if(q.empty()){
		 periods[cpu]=0;
		 return IDLE_TASK;
		}
		else {
			periods[cpu] = q.top().first;
			int sig = q.top().second; q.pop();
			return sig;
		}	
	} else { //La actual se quiere seguir ejecutando / bloqueando

			if(!q.empty()){

				if (current_pid(cpu) == IDLE_TASK) {
					periods[cpu] = q.top().first;
					int sig = q.top().second; q.pop();
					return sig;
				}
				//Si hay esperando una con mas prioridad que la actual
				if(current_pid(cpu)!= IDLE_TASK && periods[cpu] > q.top().first ){
					pair<int,int> pushear = make_pair(periods[cpu], current_pid(cpu));
					periods[cpu] = q.top().first;
					int sig = q.top().second; q.pop();
					q.push(pushear);
					return sig;
				}
			
			}
		return current_pid(cpu);
		
	}

}
