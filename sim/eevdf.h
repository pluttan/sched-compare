#pragma once

#include "scheduler.h"
#include <set>

namespace sched {

class EEVDF : public Scheduler {
public:
    const char* name() const override { return "eevdf"; }

    void on_enqueue(Task& t, double now_ms) override;
    Task* pick_next(double now_ms) override;
    void on_tick(Task& running, double dt_ms, double now_ms) override;
    void on_dequeue(Task& t, double now_ms) override;
    double current_virtual_time() const override { return V_; }

private:
    struct ByDeadline {
        bool operator()(const Task* a, const Task* b) const {
            if (a->vd != b->vd) return a->vd < b->vd;
            return a->id < b->id;
        }
    };

    std::multiset<Task*, ByDeadline> tree_;
    double V_ = 0.0;
    Task* running_ = nullptr;

    double slice_for(const Task& t) const;
    void refresh_deadline(Task& t) const;
};

}
