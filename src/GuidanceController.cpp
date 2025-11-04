//GuidanceController.cpp
#include "GuidanceController.hpp"
#include "IGuidance.hpp"
//#include "MidtermGuidance.hpp"
//#include "TerminalGuidance.hpp"


//전송 by SystemContext
void
GuidanceController::start() {
 if (running_.exchange(true)) return;
 worker_ = std::thread(&GuidanceController::GuidanceTask, this);
}

//전송 by SystemContext
void
GuidanceController::stop() {
 if (!running_.exchange(false)) return;
 if (worker_.joinable()) worker_.join();
} 

void 
GuidanceController::join() {
    if (worker_.joinable()) worker_.join();
}

//스레드 함수(유도 태스크) 
void
GuidanceController::GuidanceTask() {
    
    while (running_) {
        //현재 상태 불러오기
        double flight_time_now = getFlightTimeNow(); //현재 루프 진입 시간
        double dt = flight_time_now - previous_loop_start_time_;
        missile_state_t missile_now = missile_mgr.getCurrentMissile(flight_time_now);
        target_state_t target_now = target_mgr.getCurrentTarget(flight_time_now);
        //모드 확인과 변경 
        checkAndUpdateStrategy(missile_now, target_now);
        //
        IGuidance* cur = nullptr;
        if(mode_ == GuidanceMode::Mid){
            cur = mid_.get();
        }
        else if(mode_ == GuidanceMode::Terminal){
            cur = term_.get();
        }
        else{
            running_.store(false);
        
        }   
        
        if(running_.load() == true) {
            Vec3 a_f = cur->calculateGuidance(missile_now, target_now, dt);
            //유도탄 상태 업데이트
            previous_loop_start_time_ = flight_time_now;
            missile_mgr.updateState(missile_now, a_f, flight_time_now);  
            //일정 주기 sleep
            std::this_thread::sleep_until(Clock::now() + 200ms);
        }
        else {
            missile_mgr.updateState(missile_now, {0,0,0}, flight_time_now); //종료 상태여도 종료 시점 상태 업데이트하고 GuidanceTask return 
            return;
        } 
        
    
    }

}
//모드 확인과 변경 
 void
 GuidanceController::checkAndUpdateStrategy(missile_state_t missile_now, target_state_t target_now) {
    Vec3 R = sub3(target_now.r_t, missile_now.r_m);         
    double Rmag= norm3(R) + 1e-12; // time_now 에서 유도탄과 표적의 거리 
    std::lock_guard<std::mutex> lock(mtx_);
    switch(mode_){
        // Mid -> Terminal 
        case GuidanceMode::Mid:
            if(Rmag <= 10000.0){
                mode_ = GuidanceMode::Terminal;
            }
            break;
        // Terminal -> End
        case GuidanceMode::Terminal:
            if(Rmag <= 10.0){
                mode_ = GuidanceMode::Ended;
            }
            break;
        case GuidanceMode::Ended:
            break;
    }
}


 //현재 시각 찍고 filght time(double)으로 변환 후 반환 
 double 
 GuidanceController::getFlightTimeNow() {
    TimePoint real_time_now = Clock::now();
    double flight_time_now = std::chrono::duration_cast<Duration>(real_time_now - flight_time_).count(); 
    return flight_time_now;
}

 void 
 GuidanceController::setMode(GuidanceMode m) {
        std::lock_guard<std::mutex> lock(mtx_);
        mode_ = m;
}


void 
GuidanceController::setFlightStart(TimePoint tp) {
     flight_time_ = tp;
     previous_loop_start_time_ = 0.0;
}