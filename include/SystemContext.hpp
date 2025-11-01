//SystemContext.hpp
#pragma once
#include "MissileStateManager.hpp"
#include "TargetStateManager.hpp"
#include "GuidanceController.hpp"
#include <atomic>
class SystemContext {
public:
    SystemContext();
    void run(); //메인 스레드 실행 
    void stop();
    
    
private:
    TimePoint time_start_; // flight time 기준 시각
    //데이터 
    Vec3 initial_r_m;
    Vec3 initial_u_m;
    MissileStateManager msm_;
    TargetStateManager tsm_;
    //통신부(미정)
    //Receiver receiver_; 
    //Receiver receive_processor_;
    //유도부 
    GuidanceController guidance_controller_;

    
    //기능 
    void toIdle_(); //초기화 절차 
    bool runLaunchProcedure_(); //시작 절차 
    void startInitialGuidance();
    void startDataLink_(); //통신 태스크 실행 
    void startGuidance_(); //유도 태스크 실행 
    void waitMissionEnd_(); //종료 상태까지 대기(blocking)
    void finalizeAndReset_(); //종료 절차 
    
    //스레드 관련 
    std::atomic<bool> running_{false};
};