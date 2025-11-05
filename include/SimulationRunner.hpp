#include "TargetStateManager.hpp"
#include "atomic"
#include <thread>

class SimulationRunner {
    private:
        TargetStateManager& target_mgr_;
        std::atomic<bool> running_{false};
        std::thread worker_;
        TimePoint flight_time_{};
    public:
        SimulationRunner(TargetStateManager& target_mgr)
            :target_mgr_(target_mgr)
        {};
        void SimulationTask();
        void StartSimulation(TimePoint flight_time);
        void StopSimulation();
        double getFlightTimeNow();
};