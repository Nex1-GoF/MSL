//GuidanceController.hpp
#pragma once
#include "MissileStateManager.hpp"
#include "TargetStateManager.hpp"
#include "IGuidance.hpp"
#include "MidtermGuidance.hpp"
#include "TerminalGuidance.hpp"
#include <thread>
#include <atomic>
#include <chrono>
#include <memory>

enum class GuidanceMode { Mid, Terminal, Ended };


class GuidanceController {
private:
    //생성자에 의해 초기화 
    MissileStateManager& missile_mgr;
    TargetStateManager& target_mgr;
    TimePoint flight_time_{}; //발사 시작 시간(절대 시간)
    //모드, 상태 
    GuidanceMode mode_{GuidanceMode::Mid};
    //for mode_ 
    std::mutex mtx_;
    //전략 캐시 ->멤버 변수로 만들어두고 재사용
    std::unique_ptr<IGuidance> mid_ = std::make_unique<MidtermGuidance>();
    std::unique_ptr<IGuidance> term_ = std::make_unique<TerminalGuidance>();
    
    IGuidance* strategy_{mid_.get()}; // 현재 선택된 전략(비소유), 초기값->mid
    //스레드 관련  
    std::atomic<bool> running_{false};
    std::thread worker_;
    //유도 태스크 관련
    double previous_loop_start_time_ = 0.0; //이전 루프 진입 시간 -> dt 계산에 필요

    void GuidanceTask(); //유도 태스크 
    void checkAndUpdateStrategy(missile_state_t, target_state_t);
    double getFlightTimeNow();

public:
    //생성자 
    GuidanceController(MissileStateManager& msm, TargetStateManager& tm, TimePoint flight_time)
        : missile_mgr(msm), target_mgr(tm), flight_time_(flight_time) {};
    
    void setMode(GuidanceMode m);
    void setFlightStart(TimePoint tp);
    void setLink();
    //GuidanceTask 관리 
    void start();  
    void stop();
    void join();
};
