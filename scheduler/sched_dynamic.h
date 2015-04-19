#ifndef __SCHED_DYNAMIC_
#define __SCHED_DYNAMIC_

#include <vector>
#include <queue>
#include "basesched.h"

using namespace std;

class SchedDynamic : public SchedBase {
	public:
		SchedDynamic(std::vector<int> argn);
        	~SchedDynamic();
		virtual void initialize();
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:
		struct task {
			int pid, deadline;
			bool operator<(const task &t2) const {return deadline > t2.deadline;}
		};
		std::priority_queue<task> q;
		int ciclo; // numero de ciclo actual
};

#endif
