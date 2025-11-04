#pragma once
#include "Utils.hpp"
#include <mutex>
#include <chrono>
//표적 상태 
struct target_state_t {
    Vec3 r_t{0,0,0};   // [m] 표적 위치(측정 기준)
    Vec3 v_t{0,0,0};   // [m/s] 표적 속도(측정 기준)
    double t{0};     // 측정 기준 시각
};


class TargetStateManager {
private:
    target_state_t target_state;
    std::mutex mtx;

public:
    void updateState(Vec3 & r_t_new, Vec3 & u_t_new, double time_now); //표적 업데이트 
    target_state_t getCurrentTarget(double time_now);  //time_now의 예상 표적 상태 반환 

};