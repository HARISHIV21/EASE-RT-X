#include "diagnostic_manager.hpp"
namespace ease_rt_x {
void DiagnosticManager::record(const FaultEvent& e){ if(e.fault_id==0)return; for(std::size_t i=0;i<count_;++i){auto&r=records_[i];if(r.id==e.fault_id){++r.occurrences;r.active=e.active;r.confirmed=r.confirmed||r.occurrences>=2;return;}} if(count_<MAX_RECORDS){records_[count_++]={e.fault_id,e.type,e.severity,e.timestamp_ms,1,e.active,false};}}
void DiagnosticManager::clear(std::uint32_t id){if(auto*r=find(id))r->active=false;}
DiagnosticRecord* DiagnosticManager::find(std::uint32_t id){for(std::size_t i=0;i<count_;++i)if(records_[i].id==id)return &records_[i];return nullptr;}
const DiagnosticRecord* DiagnosticManager::find(std::uint32_t id)const{for(std::size_t i=0;i<count_;++i)if(records_[i].id==id)return &records_[i];return nullptr;}
}
