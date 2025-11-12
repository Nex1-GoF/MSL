//MissileStateManager.cpp
#include "MissileStateManager.hpp"

missile_state_t
MissileStateManager::getMissileState() {
    std::lock_guard<std::mutex> lock(mtx);
    return msl_state;
}


void
MissileStateManager::updateState( missile_state_t new_state, Vec3 new_a_f, Vec3 new_pip, double time_now, char f_status) {
    std::lock_guard<std::mutex> lock(mtx);
    msl_state.r_m = new_state.r_m; //위치 갱신 
    msl_state.u_m = new_state.u_m;  //진행 방향 단위벡터 갱신 
    msl_state.a_f_ = new_a_f;
    msl_state.pip = new_pip;
    msl_state.last_update_time = time_now; 
    msl_state.f_status = f_status;
}
//초기 유도에서 유도탄 상태 업데이트
void 
MissileStateManager::updateForInitialGuidance(double time_now) {

    std::lock_guard<std::mutex> lock(mtx);
    double time_prev = msl_state.last_update_time; 
    // 등속 모델로 cur_time으로 보정
    double dt = time_now - time_prev;
    missile_state_t corrected = msl_state; 
    corrected.r_m = add3(msl_state.r_m, scale3(scale3(msl_state.u_m, msl_state.V_m), dt)); //5초 후 위치
    corrected.last_update_time = time_now; //현재 시간 (5초)

    msl_state = corrected;
}


missile_state_t 
MissileStateManager::getCurrentMissile(double time_now) {
    std::lock_guard<std::mutex> lock(mtx);

    const double dt = time_now - msl_state.last_update_time;

    // 이전 유도탄 상태 
    missile_state_t msl_state_now = msl_state;

    // 방향 외삽: 직전 "실제 가속"을 유지해서 u를 갱신 (옵션)
    Vec3 u = msl_state.u_m;
    
    // u ← unit(u + (a_f / V) * dt)
    const Vec3 du = scale3(msl_state.a_f_, (1.0 /msl_state.V_m) * dt);
    u = unit3(add3(u, du));

    // 위치 외삽: r ← r + V * u * dt  (u는 위에서 선택한 방식)
    const Vec3 r = add3(msl_state.r_m, scale3(u, msl_state.V_m * dt));
    
    // 현재 시간(time_now)에 대한 예상 유도탄 상태 반환 
    msl_state_now.r_m = r;
    msl_state_now.u_m = u;
    msl_state_now.last_update_time = time_now; // 이 스냅샷의 기준 시각

    return msl_state_now;
}

void 
MissileStateManager::setInitialState(Vec3 u_m_init){
    std::lock_guard<std::mutex> lock(mtx);
    msl_state.u_m = u_m_init;
}


void 
MissileStateManager::setFlightSatate(char state){
    std::lock_guard<std::mutex> lock(mtx);
    msl_state.f_status = state;
}