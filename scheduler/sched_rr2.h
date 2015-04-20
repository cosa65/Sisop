#ifndef __SCHED_RR2__
#define __SCHED_RR2__

#include <vector>
#include <queue>
#include <map>
#include "basesched.h"

class SchedRR2 : public SchedBase {
	public:
		SchedRR2(std::vector<int> argn);
        	~SchedRR2();
		virtual void initialize() {};
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:
		int next(int cpu);
		std::vector<int> quantum;
		std::vector<int> quantum_status;
		std::vector<std::queue<int> > qs;
		std::map<int, int> proc_cpu; // cada proceso, guarda su cpu

};

#endif
