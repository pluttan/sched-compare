#include "scheduler.h"
#include <algorithm>
#include <iomanip>

namespace sched {

void run_simulation(Scheduler& sch, std::vector<Task>& tasks,
                    const SimConfig& cfg, std::ostream& out) {
    std::sort(tasks.begin(), tasks.end(),
              [](const Task& a, const Task& b){ return a.arrival_ms < b.arrival_ms; });

    out << "time_ms,task_id,task_name,event,vruntime,vd,V,remaining_ms\n";
    out << std::fixed << std::setprecision(4);

    auto emit = [&](double t, const Task* task, const char* ev) {
        double vrt = task ? task->vruntime : 0.0;
        double vd = task ? task->vd : 0.0;
        out << t << ","
            << (task ? task->id : -1) << ","
            << (task ? task->name : "") << ","
            << ev << ","
            << vrt << "," << vd << ","
            << sch.current_virtual_time() << ","
            << (task ? task->remaining_ms : 0.0) << "\n";
    };

    double t = 0.0;
    size_t arrive_idx = 0;
    Task* last_running = nullptr;

    while (t <= cfg.max_time_ms + 1e-9) {
        while (arrive_idx < tasks.size() && tasks[arrive_idx].arrival_ms <= t + 1e-9) {
            Task& task = tasks[arrive_idx];
            if (!task.arrived) {
                task.arrived = true;
                sch.on_enqueue(task, t);
                emit(t, &task, "arrival");
            }
            ++arrive_idx;
        }

        Task* running = sch.pick_next(t);

        if (running != last_running) {
            if (last_running && !last_running->finished) emit(t, last_running, "preempt");
            if (running) {
                if (running->first_run_ms < 0) running->first_run_ms = t;
                emit(t, running, "start");
            }
            last_running = running;
        }

        if (!running) {
            t += cfg.tick_ms;
            continue;
        }

        double dt = cfg.tick_ms;
        if (running->remaining_ms < dt) dt = running->remaining_ms;

        sch.on_tick(*running, dt, t);
        running->remaining_ms -= dt;
        running->total_runtime_ms += dt;
        t += dt;

        if (running->remaining_ms <= 1e-9) {
            running->finished = true;
            running->finish_ms = t;
            sch.on_dequeue(*running, t);
            emit(t, running, "finish");
            last_running = nullptr;
        }

        if (cfg.emit_ticks) emit(t, running, "tick");
    }
}

}
