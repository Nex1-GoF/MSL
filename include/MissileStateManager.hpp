//MissileStateManager.hpp
#pragma once
#include "Utils.hpp"
#include <mutex>
#include <chrono>

//유도탄 상태 구조체  
struct missile_state_t {
    Vec3 r_m{0,0,0};
    Vec3 u_m{0,0,0}; 
    double V_m = 1000.0;  //속력
    double last_update_time{0}; //flight_time 기준 시간 
    Vec3 a_f_{0,0,0};           // [m/s^2] 실제(응답/포화 후) 가속
    Vec3 pip{0,0,0};
    char f_status{1}; //비행 상태(1:발사준비 2:초기 3:중기 4:종말 5:종료)
    uint8_t t_status{1}; //텔레메트리
};


class MissileStateManager {
private:
    missile_state_t msl_state;
    std::mutex mtx;
public:
    
    //현재 유도탄 상태 return    
    missile_state_t getMissileState() ;
    missile_state_t getCurrentMissile(double time_now);
    void updateState(missile_state_t new_state, Vec3 new_a_f, Vec3 new_pip, double time_now,char f_status);
    void setInitialState(Vec3 u_m_initial); //초기 pip를 통해 계산한 초기 u_m 업데이트 
    void setFlightSatate(char f_status); //f_status 설정
    void updateForInitialGuidance(double time_now); //초기 유도 등속 운동 업데이트 
};