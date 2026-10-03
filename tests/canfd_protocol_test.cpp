#include <cassert>
#include "communication/canfd_interface.hpp"
using namespace ease_rt_x;
int main(){CanFdInterface i;CanFdFrame f{};f.id=static_cast<unsigned>(CanFdMessageId::VEHICLE_STATE);f.length=16;f.timestamp_ms=100;f.sequence=0;auto a=i.process(f,105,20);assert(a.accepted());f.sequence=1;f.timestamp_ms=106;auto b=i.process(f,110,20);assert(b.accepted());f.sequence=4;auto c=i.process(f,111,20);assert(!c.accepted());assert(i.statistics().accepted==2);assert(i.statistics().rejected==1);return 0;}
