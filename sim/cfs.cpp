#include "cfs.h"
#include <algorithm>

namespace sched {

void CFS::on_enqueue(Task& t, double now_ms) {
    double floor = min_vruntime_ - 3.0;
    if (t.vruntime < floor) t.vruntime = floor;
    tree_.insert(&t);
}

Task* CFS::pick_next(double now_ms) {
    if (running_ && !running_->finished && running_slice_ms_ > 0.0) {
        return running_;
    }
    if (running_ && !running_->finished) {
        tree_.insert(running_);
    }
    if (tree_.empty()) { running_ = nullptr; return nullptr; }

    auto it = tree_.begin();
    Task* t = *it;
    tree_.erase(it);
    running_ = t;

    int n = static_cast<int>(tree_.size()) + 1;
    running_slice_ms_ = t->base_slice_ms(6.0, n, 0.75);
    return t;
}

void CFS::on_tick(Task& running, double dt_ms, double now_ms) {
    double vdelta = dt_ms * (double)Task::NICE_0_WEIGHT / (double)running.weight();
    running.vruntime += vdelta;
    running_slice_ms_ -= dt_ms;

    if (!tree_.empty()) {
        double tree_min = (*tree_.begin())->vruntime;
        min_vruntime_ = std::max(min_vruntime_, std::min(running.vruntime, tree_min));
    } else {
        min_vruntime_ = std::max(min_vruntime_, running.vruntime);
    }
}

void CFS::on_dequeue(Task& t, double now_ms) {
    if (running_ == &t) {
        running_ = nullptr;
        running_slice_ms_ = 0.0;
    } else {
        tree_.erase(&t);
    }
}

}
