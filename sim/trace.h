#pragma once

#include "task.h"
#include <vector>
#include <string>

namespace sched {

std::vector<Task> load_trace_csv(const std::string& path);

}
