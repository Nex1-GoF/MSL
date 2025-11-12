//TaskManager.cpp
#include "TaskManager.hpp"


void 
TaskManager::start() {

    std::cout << "[태스크 매니저] 태스크 시작 요청 수신" << std::endl;
     // 콜백 등록
    guidance_controller_.setTerminationCallback([this]() {
        this->stop();
    });

    datalink_manager_.setTerminationCallback([this]() {
        this->stop();
    });
    //스레드 실행 
    datalink_manager_.setDataLink();
    datalink_manager_.startDataLink();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    guidance_controller_.startGuidanceTask();
}
void 
TaskManager::stop() {

     if (stopped_) return;  // 중복 방지
    stopped_ = true;

    guidance_controller_.stopGuidanceTask();
    datalink_manager_.stopDataLink();
}

void 
TaskManager::join() {

    guidance_controller_.join();
    datalink_manager_.joinDataLink();

}