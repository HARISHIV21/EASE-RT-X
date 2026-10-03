#include "canfd_interface.hpp"
namespace ease_rt_x {CanFdValidationResult CanFdInterface::process(const CanFdFrame&f,std::uint32_t now,std::uint32_t timeout){++statistics_.received;auto r=protocol_.validate(f,now,timeout);if(r.accepted()){protocol_.accept(f);++statistics_.accepted;consecutive_rejections_=0;}else{++statistics_.rejected;++consecutive_rejections_;}return r;}}
