//SystemContext.hpp
#pragma once
#include "MissileStateManager.hpp"
#include "TargetStateManager.hpp"
#include "GuidanceController.hpp"
#include "Utils.hpp"
#include "SimulationRunner.hpp"
#include "DataLinkManager.hpp"
#include "TaskManager.hpp"
#include "HeaderPacket.hpp"
#include "KeyPacket.hpp"
#include "PipPacket.hpp"
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
    //통신부
    SimulationRunner simulation_runner_;
    DataLinkManager datalink_manager_;
    //유도부 
    GuidanceController guidance_controller_;
    TaskManager task_manager_; 
    
    //기능 
    void toIdle_(); //초기화 절차 
    bool runLaunchProcedure_(); //발사 준비 절차 
    Vec3 getInitialPIP_(); //초기 PIP 계산 
    void startInitialGuidance(); //초기 유도 
    void startGuidance_(); //유도 태스크 실행 
    void waitMissionEnd_(); //종료 상태까지 대기(blocking)
    void finalizeAndReset_(); //종료 절차 
    void runSimulation_();
    //스레드 관련 
    std::atomic<bool> running_{false};

    //발사절차 관련
    static constexpr int MAX_EVENTS = 10;
    static constexpr int MAXLINE = 1024;
    int fd_rx_{-1};
    int fd_tx_{-1};
    const char* s_id_ = "M001";
    const char* d_id_ = "C001";
    //C2 ip, port 
    char dest_ip_[16] = "127.0.0.1";
    int dest_port_ = 8001;
    //MSL ip, port 
    char ip_rx_[16] = "127.0.0.1";
    int port_rx_ = 9011;

    char ip_tx_[16] = "127.0.0.1";
    int port_tx_ = 9012;

};

