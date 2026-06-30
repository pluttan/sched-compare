#pragma once

#include <cmath>
#include <string>

namespace sched {

struct Task {
    int id = 0;
    std::string name;

    double arrival_ms = 0.0;
    double burst_ms = 0.0;
    int nice = 0;
    int latency_nice = 0;

    double remaining_ms = 0.0;
    double vruntime = 0.0;
    double ve = 0.0;
    double vd = 0.0;
    double slice_remaining_ms = 0.0;
    double total_runtime_ms = 0.0;
    double first_run_ms = -1.0;
    double finish_ms = -1.0;

    bool arrived = false;
    bool finished = false;

    static constexpr int NICE_0_WEIGHT = 1024;

    static int weight_from_nice(int n) {
        static const int prio_to_weight[40] = {
            88761, 71755, 56483, 46273, 36291,
            29154, 23254, 18705, 14949, 11916,
             9548,  7620,  6100,  4904,  3906,
             3121,  2501,  1991,  1586,  1277,
             1024,   820,   655,   526,   423,
              335,   272,   215,   172,   137,
              110,    87,    70,    56,    45,
               36,    29,    23,    18,    15,
        };
        int idx = n + 20;
        if (idx < 0) idx = 0;
        if (idx > 39) idx = 39;
        return prio_to_weight[idx];
    }

    int weight() const { return weight_from_nice(nice); }

    double base_slice_ms(double sched_latency_ms, int n_runnable, double min_granularity_ms) const {
        double s = sched_latency_ms / std::max(1, n_runnable);
        return std::max(s, min_granularity_ms);
    }
};

struct Event {
    enum Kind { ARRIVAL, START, PREEMPT, FINISH, TICK };
    double time_ms;
    Kind kind;
    int task_id;
    double vruntime_or_vd;
};

}
