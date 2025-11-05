#include "SimulationRunner.hpp"

void 
SimulationRunner::SimulationTask() {
    
    while(running_){
        double flight_time_now = getFlightTimeNow();
        target_state_t current_target = target_mgr_.getCurrentTarget(flight_time_now);
        Vec3 r_t_now = current_target.r_t;
        Vec3 u_t_now = current_target.v_t;
        target_mgr_.updateState(r_t_now, u_t_now, flight_time_now);
        
        std::this_thread::sleep_until(Clock::now() + 200ms);
    }
}


void 
SimulationRunner::StartSimulation(TimePoint flight_time) {
    flight_time_ = flight_time;

    if (running_.exchange(true)) return;
    worker_ = std::thread(&SimulationRunner::SimulationTask, this);
}
        
    
void
SimulationRunner::StopSimulation() {
    if (!running_.exchange(false)) return;
    if (worker_.joinable()) worker_.join();
}


double 
SimulationRunner::getFlightTimeNow() {
    TimePoint real_time_now = Clock::now();
    double flight_time_now = std::chrono::duration_cast<Duration>(real_time_now - flight_time_).count(); 
    return flight_time_now;
}