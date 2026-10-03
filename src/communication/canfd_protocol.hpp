#pragma once
#include "canfd_message.hpp"
namespace ease_rt_x { class CanFdProtocol{public:CanFdValidationResult validate(const CanFdFrame&,std::uint32_t,std::uint32_t)const;void accept(const CanFdFrame&);void reset();private:std::uint16_t expected_sequence_{0};bool is_known_id(std::uint32_t)const;};}
