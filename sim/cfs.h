#pragma once

#include "scheduler.h"
#include <set>

namespace sched {

class CFS : public Scheduler {
public:
    const char* name() const override { return "cfs"; }

    void on_enqueue(Task& t, double now_ms) override;
    Task* pick_next(double now_ms) override;
    void on_tick(Task& running, double dt_ms, double now_ms) override;
    void on_dequeue(Task& t, double now_ms) override;
    double current_virtual_time() const override { return min_vruntime_; }

private:
    struct ByVruntime {
        bool operator()(const Task* a, const Task* b) const {
            if (a->vruntime != b->vruntime) return a->vruntime < b->vruntime;
            return a->id < b->id;
        }
    };

    std::multiset<Task*, ByVruntime> tree_;
    double min_vruntime_ = 0.0;
    Task* running_ = nullptr;
    double running_slice_ms_ = 0.0;
};

}
