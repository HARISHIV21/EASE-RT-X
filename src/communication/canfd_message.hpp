#pragma once
#include <cstdint>
namespace ease_rt_x {
enum class CanFdMessageId:std::uint32_t{SENSOR_STATE=0x100,VEHICLE_STATE=0x110,CONTROL_COMMAND=0x120,HEALTH_STATUS=0x130,DIAGNOSTIC=0x140,FAULT_EVENT=0x150,SYSTEM_MODE=0x160};
struct CanFdFrame{std::uint32_t id{0};std::uint8_t data[64]{};std::uint8_t length{0};std::uint32_t timestamp_ms{0};std::uint16_t sequence{0};};
struct CanFdValidationResult{bool valid_id{false};bool valid_length{false};bool valid_sequence{false};bool fresh{false};bool accepted()const{return valid_id&&valid_length&&valid_sequence&&fresh;}};
}
