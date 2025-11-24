#ifndef GUIDANCECONFIG_HPP
#define GUIDANCECONFIG_HPP

#include <cstdint>

constexpr double Vm_cfg = 3000.0; //미사일 속력 
constexpr double distance_terminal_cfg = 10000.0; //종말 유도 진입 판단 기준 거리, 텔레메트리 seeker on 기준 거리 
constexpr double distance_detonate_cfg = 50.0; //기폭 판단 기준거리  
constexpr double distance_tdd_on_cfg = 3000.0; //텔레메트리 seeker on 기준 거리 
constexpr double K_ZEM_cfg   = 0.5; //midterm guidance.cpp, terminal guidance.cpp
constexpr double N_fal_cfg   = 4.0; //midterm guidance.cpp 
constexpr double N_near_cfg  = 10.0; //midterm guidance.cpp
constexpr double N0_cfg      = 3.0; // terminal guidance.cpp
constexpr double Kn_cfg      = 2.5; //terminal guidance.cpp
constexpr double tau_cfg     = 0.15; //guidancefunction.cpp > runautopilot



#endif

/*
          PN_SIGN=+1; N_far=4; N_near=10; K_ZEM=0.5;
            R_GAIN_FAR=1200; R_GAIN_NEAR=300; tgo_far=10; tgo_near=2;
*/