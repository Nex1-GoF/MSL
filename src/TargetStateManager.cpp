#include "TargetStateManager.hpp"

//수신 스레드에서 수신한 표적 정보 업데이트 
void
TargetStateManager::updateState(Vec3 & r_t_new, Vec3 & v_t_new, double time_now) {
    target_state.r_t = r_t_new;
    target_state.v_t = v_t_new;
    target_state.t = time_now;
}

target_state_t
TargetStateManager::getCurrentTarget(double time_now) {
    double time_prev = target_state.t;
     std::lock_guard<std::mutex> lock(mtx);
    // 등속 모델로 cur_time으로 보정
    double dt = time_now - time_prev;
    target_state_t corrected = target_state; 
    corrected.r_t = add3(target_state.r_t, scale3(target_state.v_t, dt));
    corrected.t   = time_now;
    return corrected; //시간 cur_time에 대한 예상 표적 정보 반환
}