//GuidanceController.cpp
#include "GuidanceController.hpp"
#include "IGuidance.hpp"

//#include "MidtermGuidance.hpp"
//#include "TerminalGuidance.hpp"


void GuidanceController::setTerminationCallback(Callback cb) {
    termination_callback_ = cb;
}


//전송 by SystemContext
void
GuidanceController::startGuidanceTask() {
    std::cout << "[유도 컨트롤러] startGuidanceTask 진입" << std::endl;
 if (running_.exchange(true)) return;
 worker_ = std::thread(&GuidanceController::GuidanceTask, this);
}

//전송 by SystemContext
void
GuidanceController::stopGuidanceTask() {
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

    std::cout << "[유도 태스크 시작]" << std::endl;
    
    while (running_) {
        //현재 상태 불러오기
        double flight_time_now = getFlightTimeNow(); //현재 루프 진입 시간
        double dt = flight_time_now - previous_loop_start_time_;
        missile_state_t missile_now = missile_mgr.getCurrentMissile(flight_time_now);
        target_state_t target_now = target_mgr.getCurrentTarget(flight_time_now);
        char cur_f_status = missile_now.f_status;
        
        /*------------------------------유도 로그용--------------------------------*/
        Vec3 R = sub3(target_now.r_t, missile_now.r_m);         
        double Rmag= norm3(R) + 1e-12; //유도탄, 표적 거리
        std::cout << "time: " << flight_time_now <<" distance: " << Rmag << " mode: " << cur_f_status << " [x: " << missile_now.r_m[0] << " y: " <<missile_now.r_m[1] <<" z: " << missile_now.r_m[2] << "]" << std::endl;
        /*------------------------------------------------------------------------*/
       
        //모드 확인과 변경 
        checkAndUpdateStrategy(missile_now, target_now);
        //
        IGuidance* cur = nullptr;
        if(mode_ == GuidanceMode::Mid){
            cur = mid_.get();
            cur_f_status = 3;
        }
        else if(mode_ == GuidanceMode::Terminal){
            cur = term_.get();
            cur_f_status = 4;
        }
        else{
            //종료 이벤트 
            std::cout << "[기폭 성공]" << std::endl;
            cur_f_status = 5;
            missile_mgr.updateState(missile_now, {0,0,0},{0,0,0}, flight_time_now, cur_f_status); //종료 시점의 상태 갱신 
            if (termination_callback_) termination_callback_(); // TaskManager.stop() 호출 -> 모든 태스크 종료(guidance, datalink, cmd) 
        }   
        
        //계산 결과 반영 
        if(running_.load() == true) {
            //현재 모드에 맞는 유도 알고리즘 계산
            Vec3 new_a_f = cur->calculateGuidance(missile_now, target_now, dt);
            Vec3 new_pip = getCurrentPIP(missile_now,target_now);
            //유도탄 상태 업데이트
            previous_loop_start_time_ = flight_time_now;
            missile_mgr.updateState(missile_now, new_a_f,new_pip, flight_time_now, cur_f_status);  
            //일정 주기 sleep
            std::this_thread::sleep_until(Clock::now() + 200ms);
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
            if(Rmag <= 50.0){
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


Vec3
GuidanceController::getCurrentPIP(const missile_state_t& m, const target_state_t& t) const
{
    // 해석적 t_go 해: (v_t·v_t - V_m^2) τ^2 + 2 (R·v_t) τ + (R·R) = 0
    InterceptSolution sol;
    const Vec3 R = sub3(t.r_t, m.r_m);
    const double V_m = m.V_m;
    const double a = dot3(t.v_t, t.v_t) - V_m * V_m;
    const double b = 2.0 * dot3(R, t.v_t);
    const double c = dot3(R, R);

    if (std::fabs(a) < 1e-12) {
        if (b < 0.0) {
            sol.tgo = -c / b;
            sol.ok = (sol.tgo > 0.0);
        }
    }
    else {
        const double D = b * b - 4.0 * a * c;
        if (D >= 0.0) {
            const double rt = std::sqrt(D);
            const double t1 = (-b - rt) / (2.0 * a);
            const double t2 = (-b + rt) / (2.0 * a);
            // 양의 해 중 작은 것
            double tau = 1e100;
            if (t1 > 1e-6) tau = std::min(tau, t1);
            if (t2 > 1e-6) tau = std::min(tau, t2);
            if (tau < 1e50) { sol.tgo = tau; sol.ok = true; }
        }
    }

    if (!sol.ok) {
        // 기하학적 근사 fallback: t_go ≈ R / Vc(LOS)
        const double Rmag = norm3(R);
        const Vec3 rhat = (Rmag < 1e-12) ? Vec3{ 0,0,0 } : scale3(R, 1.0 / Rmag);
        const Vec3 v_rel_geom = sub3(t.v_t, scale3(m.u_m, m.V_m));
        const double Vc_geom = std::max(-dot3(v_rel_geom, rhat), 10.0); // 안전 하한
        sol.tgo = clip(Rmag / Vc_geom, 0.05, 120.0);
        sol.ok = true; // 근사는 항상 유효로 간주
    }
    sol.pip = add3(t.r_t, scale3(t.v_t, sol.tgo));
    return sol.pip;
}