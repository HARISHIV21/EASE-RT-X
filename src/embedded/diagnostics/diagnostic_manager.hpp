#pragma once
#include "../../common/system_types.hpp"
#include <array>
namespace ease_rt_x {
struct DiagnosticRecord { std::uint32_t id{0}; FaultType type{FaultType::NONE}; FaultSeverity severity{FaultSeverity::INFORMATIONAL}; std::uint64_t first_seen_ms{0}; std::uint32_t occurrences{0}; bool active{false}; bool confirmed{false}; };
class DiagnosticManager {
public:
    void record(const FaultEvent& e);
    void clear(std::uint32_t id);
    DiagnosticRecord* find(std::uint32_t id);
    const DiagnosticRecord* find(std::uint32_t id) const;
    std::size_t size() const { return count_; }
    const DiagnosticRecord* records() const { return records_.data(); }
private:
    static constexpr std::size_t MAX_RECORDS=32; std::array<DiagnosticRecord,MAX_RECORDS> records_{}; std::size_t count_{0};
};
}
