#include "timing_monitor.hpp"
namespace ease_rt_x { TimingSample TimingMonitor::measure(std::uint32_t s,std::uint32_t e,std::uint32_t p,std::uint32_t d)const{TimingSample x;x.execution_time_ticks=e-s;x.period_ticks=s-p;x.deadline_ticks=d;x.deadline_missed=x.execution_time_ticks>d;return x;} }
