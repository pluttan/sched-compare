#include "trace.h"
#include "cfs.h"
#include "eevdf.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <cstring>

using namespace sched;

static void print_usage() {
    std::cerr <<
"Usage: sim <cfs|eevdf> <trace.csv> [--max-time MS] [--tick MS] [--out FILE]\n"
"\n"
"  <cfs|eevdf>   scheduler to simulate\n"
"  <trace.csv>   task trace file (name,arrival_ms,burst_ms[,nice[,latency_nice]])\n"
"  --max-time    simulation horizon in ms (default 200)\n"
"  --tick        tick granularity in ms (default 0.1)\n"
"  --out         write CSV to file instead of stdout\n";
}

int main(int argc, char** argv) {
    if (argc < 3) { print_usage(); return 1; }

    std::string algo = argv[1];
    std::string trace = argv[2];
    SimConfig cfg;
    std::string out_path;

    for (int i = 3; i < argc; ++i) {
        if (!std::strcmp(argv[i], "--max-time") && i+1 < argc) { cfg.max_time_ms = std::stod(argv[++i]); }
        else if (!std::strcmp(argv[i], "--tick") && i+1 < argc) { cfg.tick_ms = std::stod(argv[++i]); }
        else if (!std::strcmp(argv[i], "--out") && i+1 < argc) { out_path = argv[++i]; }
        else { std::cerr << "unknown arg: " << argv[i] << "\n"; return 1; }
    }

    std::unique_ptr<Scheduler> sch;
    if (algo == "cfs") sch = std::make_unique<CFS>();
    else if (algo == "eevdf") sch = std::make_unique<EEVDF>();
    else { std::cerr << "unknown algo: " << algo << "\n"; return 1; }

    auto tasks = load_trace_csv(trace);

    std::ofstream file;
    std::ostream* out = &std::cout;
    if (!out_path.empty()) { file.open(out_path); out = &file; }

    run_simulation(*sch, tasks, cfg, *out);

    std::cerr << "done: " << sch->name() << " trace=" << trace
              << " tasks=" << tasks.size() << "\n";
    for (const auto& t : tasks) {
        std::cerr << "  [" << t.id << "] " << t.name
                  << " arr=" << t.arrival_ms
                  << " burst=" << t.burst_ms
                  << " first_run=" << t.first_run_ms
                  << " finish=" << t.finish_ms
                  << " wait=" << (t.first_run_ms < 0 ? -1.0 : t.first_run_ms - t.arrival_ms)
                  << " turnaround=" << (t.finish_ms < 0 ? -1.0 : t.finish_ms - t.arrival_ms)
                  << "\n";
    }
    return 0;
}
