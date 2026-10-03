#pragma once
#include "canfd_protocol.hpp"
namespace ease_rt_x {struct CanFdStatistics{std::uint32_t received{0},accepted{0},rejected{0};};class CanFdInterface{public:CanFdValidationResult process(const CanFdFrame&,std::uint32_t,std::uint32_t);const CanFdStatistics&statistics()const{return statistics_;} std::uint32_t consecutive_rejections()const{return consecutive_rejections_;} bool healthy()const{return consecutive_rejections_<3;}private:CanFdProtocol protocol_;CanFdStatistics statistics_{}; std::uint32_t consecutive_rejections_{0};};}
