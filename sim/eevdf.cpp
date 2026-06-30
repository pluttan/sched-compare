#include "eevdf.h"
#include <algorithm>

namespace sched {

double EEVDF::slice_for(const Task& t) const {
    double base = 3.0;
    double ln = static_cast<double>(t.latency_nice);
    double factor = std::pow(1.25, ln);
    double s = base * factor;
    if (s < 0.3) s = 0.3;
    if (s > 20.0) s = 20.0;
    return s;
}

void EEVDF::refresh_deadline(Task& t) const {
    double s = slice_for(t);
    double w = t.weight();
    t.vd = t.ve + s * (double)Task::NICE_0_WEIGHT / w;
    t.slice_remaining_ms = s;
}

void EEVDF::on_enqueue(Task& t, double now_ms) {
    t.ve = V_;
    refresh_deadline(t);
    tree_.insert(&t);
}

Task* EEVDF::pick_next(double now_ms) {
    Task* tree_best = nullptr;
    for (auto* t : tree_) {
        if (t->ve <= V_ + 1e-9) { tree_best = t; break; }
    }

    if (running_ && !running_->finished && running_->slice_remaining_ms > 1e-9) {
        if (!tree_best || running_->vd <= tree_best->vd) {
            return running_;
        }
    }

    if (running_ && !running_->finished) {
        tree_.insert(running_);
        running_ = nullptr;
    }

    Task* best = tree_best;
    if (!best && !tree_.empty()) best = *tree_.begin();
    if (!best) return nullptr;

    tree_.erase(best);
    running_ = best;
    return best;
}

void EEVDF::on_tick(Task& running, double dt_ms, double now_ms) {
    int total_w = running.weight();
    for (auto* t : tree_) total_w += t->weight();
    if (total_w <= 0) total_w = Task::NICE_0_WEIGHT;

    double dV = dt_ms * (double)Task::NICE_0_WEIGHT / (double)total_w;
    V_ += dV;

    double vdelta = dt_ms * (double)Task::NICE_0_WEIGHT / (double)running.weight();
    running.ve += vdelta;
    running.vruntime += vdelta;
    running.slice_remaining_ms -= dt_ms;

    if (running.slice_remaining_ms <= 1e-9) {
        refresh_deadline(running);
    }
}

void EEVDF::on_dequeue(Task& t, double now_ms) {
    if (running_ == &t) {
        running_ = nullptr;
    } else {
        tree_.erase(&t);
    }
}

}
