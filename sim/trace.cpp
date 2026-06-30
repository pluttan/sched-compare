#include "trace.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace sched {

static std::string trim(std::string s) {
    auto not_space = [](int c){ return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
    s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
    return s;
}

std::vector<Task> load_trace_csv(const std::string& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("cannot open trace: " + path);

    std::vector<Task> out;
    std::string line;
    int lineno = 0;
    int next_id = 1;
    bool saw_header = false;

    while (std::getline(f, line)) {
        ++lineno;
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        if (!saw_header && (line.find("arrival") != std::string::npos ||
                             line.find("burst") != std::string::npos)) {
            saw_header = true;
            continue;
        }
        saw_header = true;

        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> cells;
        while (std::getline(ss, cell, ',')) cells.push_back(trim(cell));
        if (cells.size() < 3) {
            throw std::runtime_error("bad trace row " + std::to_string(lineno));
        }

        Task t;
        t.id = next_id++;
        t.name = cells[0];
        t.arrival_ms = std::stod(cells[1]);
        t.burst_ms = std::stod(cells[2]);
        t.nice = cells.size() > 3 && !cells[3].empty() ? std::stoi(cells[3]) : 0;
        t.latency_nice = cells.size() > 4 && !cells[4].empty() ? std::stoi(cells[4]) : 0;
        t.remaining_ms = t.burst_ms;
        out.push_back(t);
    }
    return out;
}

}
