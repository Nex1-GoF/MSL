#pragma once
#include "GuidanceController.hpp"
#include "DataLinkManager.hpp"

class TaskManager {
public:
    TaskManager(GuidanceController& guidance, DataLinkManager& datalink)
        : guidance_controller_(guidance), datalink_manager_(datalink){};
       
   ~TaskManager() = default;

    void start();
    void stop();
    void join();

private:
    GuidanceController& guidance_controller_;
    DataLinkManager& datalink_manager_;
    bool stopped_ = false;
};