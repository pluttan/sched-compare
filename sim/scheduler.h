#pragma once

#include "task.h"
#include <vector>
#include <ostream>

namespace sched {

struct SimConfig {
    double tick_ms = 0.1;
    double sched_latency_ms = 6.0;
    double min_granularity_ms = 0.75;
    double max_time_ms = 200.0;
    bool emit_ticks = false;
};

class Scheduler {
public:
    virtual ~Scheduler() = default;

    virtual const char* name() const = 0;

    virtual void on_enqueue(Task& t, double now_ms) = 0;

    virtual Task* pick_next(double now_ms) = 0;

    virtual void on_tick(Task& running, double dt_ms, double now_ms) = 0;

    virtual void on_dequeue(Task& t, double now_ms) = 0;

    virtual double current_virtual_time() const = 0;
};

void run_simulation(Scheduler& sched, std::vector<Task>& tasks,
                    const SimConfig& cfg, std::ostream& out);

}
