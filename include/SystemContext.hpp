//SystemContext.hpp
#pragma once
#include "MissileStateManager.hpp"
#include "TargetStateManager.hpp"
#include "GuidanceController.hpp"
#include "Utils.hpp"
#include <thread>
#include <atomic>
#include <chrono>
#include <memory>

class SystemContext {
public:
    SystemContext();//생성자 
    void run(); //메인 스레드 실행 
    void stop();
    
    
private:
    TimePoint time_start_; // flight time 기준 시각
    //데이터 
    MissileStateManager msm_;
    TargetStateManager tsm_;
    //통신부(구현 x)
    //Receiver receiver_; 
    //Receiver receive_processor_;
    //유도부 
    GuidanceController guidance_controller_;
    
    
    //기능 
    void toIdle_(); //초기화 절차 
    bool runLaunchProcedure_(); //발사 준비 절차 
    Vec3 getInitialPIP_(); //초기 PIP 계산 
    void startInitialGuidance(); //초기 유도 
    void startDataLink_(); //통신 태스크 실행 
    void startGuidance_(); //유도 태스크 실행 
    void waitMissionEnd_(); //종료 상태까지 대기(blocking)
    void finalizeAndReset_(); //종료 절차 
    
    //스레드 관련 
    std::atomic<bool> running_{false};
};

