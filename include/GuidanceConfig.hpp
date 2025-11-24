#ifndef GUIDANCECONFIG_HPP
#define GUIDANCECONFIG_HPP

#include <cstdint>

constexpr double Vm_cfg = 1200.0; //미사일 속력 
constexpr double distance_terminal_cfg = 10000.0; //종말 유도 진입 판단 기준 거리, 텔레메트리 seeker on 기준 거리 
constexpr double distance_detonate_cfg = 1000.0; //기폭 판단 기준거리  
constexpr double distance_tdd_on_cfg = 3000.0; //텔레메트리 seeker on 기준 거리 
constexpr double K_ZEM_cfg   = 3.5; //midterm guidance.cpp, terminal guidance.cpp
constexpr double N_fal_cfg   = 3.0; //midterm guidance.cpp 
constexpr double N_near_cfg  = 4.0; //midterm guidance.cpp
constexpr double N0_cfg      = 3.0; // terminal guidance.cpp
constexpr double Kn_cfg      = 0.8; //terminal guidance.cpp
constexpr double tau_cfg     = 0.2; //guidancefunction.cpp > runautopilot



#endif