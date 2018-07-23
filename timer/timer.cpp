#include "timer.h"

static std::atomic<int64_t> Timer::s_nCountCreate(0);
