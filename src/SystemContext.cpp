#include "SystemContext.hpp"
#include <iostream>

//생성자 구현 
SystemContext::SystemContext()
    :time_start_{}, //flight_time 기준 시각            
    msm_{},        
    tsm_{},    
    simulation_runner_{tsm_},
    datalink_manager_{tsm_, msm_},
    guidance_controller_{msm_, tsm_, time_start_} 
{ }
;

//메인 스레드 
void
SystemContext::run() {
    running_ = true;
    while(running_) { 
        //toIdle_(); //소켓등 시스템 자원 초기화
        //if(!runLaunchProcedure_()) continue; //발사 절차->중단 시 발사 대기 상태 재진입 
        //startInitialGuidance(); //초기 유도 수행
        std::string start_cmd;
        std::cout << "enter any key: " << std::endl;
        std::cin >> start_cmd;
        startDataLink_(); //통신 관련 태스크 실행 
        startGuidance_(); //유도 태스크 실행 -> 상태관리, 전략 설정은 GuidanceContoller 가 담당 
        waitMissionEnd_(); //종료 이벤트까지 대기 
        finalizeAndReset_(); // 종료 절차 
    //runSimulation_();
    }
}
/*---------------------------------------------------------------------------------------*/
void 
SystemContext::toIdle_() {
    //1. 멤버 객체 초기화 
    //2. 발사 절차를 위한 소켓 설정 
}


// bool 
// SystemContext::runLaunchProcedure_() {
//     //0. wait 

//     //1. 발사 절차 (핑퐁)
    


//     /*
//         구현 예정 
//     */

//     //2. 발사 명령 + 초기 포착 지점 수신 이벤트 
//     /*
//         구현 예정 
//     */


//     //  "초기 포작 지점"과 "유도탄 초기 위치"(rm, 발사대 위치)로 유도탄 초기 방향(um) 계산(등속 직선 운동을 위해)
//     Vec3 u_m_init = getInitialPIP_();
//     msm_.setInitialState(u_m_init);
//     //  SystemContext의 time_start_ 초기화 (시간 동기화용 -> real time을 flight time으로 변환할 때 기준 시각)
//     time_start_ = Clock::now(); 
//     //유도 태스크 실행 객체의 기준 시간 초기화  
//     guidance_controller_.setFlightStart(time_start_);
//     return true;
// }

void 
SystemContext::startInitialGuidance() {
   /*확정 x*/
   while(true) {

        TimePoint real_time_now = Clock::now();
        double flight_time_now = std::chrono::duration_cast<Duration>(real_time_now - time_start_).count(); 
        if(flight_time_now >= 5.0) {
            msm_.updateForInitialGuidance(flight_time_now); //현재 시각 기준으로 msl 업데이트 
            break; 
        }
        /* 구현 x -100 ms 동안 sleep" */
        std::this_thread::sleep_until(Clock::now() + 200ms);
   }
   
   return;
}

void 
SystemContext::startDataLink_() {
    datalink_manager_.setDataLink();
    datalink_manager_.startDataLink();
    return;
}
void 
SystemContext::startGuidance_() {
    //유도 태스크 start 
    guidance_controller_.startGuidanceTask();
    return;
}

void 
SystemContext::waitMissionEnd_() {
    //GuidanceTask 리턴 이벤트 대기,join() 함수는 GuidanceTask가 return하면 return
    guidance_controller_.join(); //유도 태스크 종료 대기  
    datalink_manager_.stopDataLink(); // 데이터링크 태스크 종료 
    return;
}

void 
SystemContext::finalizeAndReset_() {
    //종료 시점 유도탄 정보, flight time 전송 (GuidanceController가 종료시점의 상태 업데이트 역할)
    //여기서는 그냥 불러와서 전송만 
    /*통신부 완료 후 구현 예정 */
}
/*---------------------------------------------------------------------------------------*/
/*
Vec3
SystemContext::getInitialPIP_() {
    //수신한 초기 표적 정보, 유도탄 초기 위치로 u_m_initial 계산 후 return  
   
}
*/
void 
SystemContext::runSimulation_() {
    //표적 초기 상태 설정 
    Vec3 r_t_initial = {12000.0, 12000.0, 22000.0}; //초기 위치 (0초에서의 위치)
    Vec3 v_t_initial = {-600.0, -600.0, 0}; //등속 
    tsm_.updateState(r_t_initial, v_t_initial, 0);
    //wait to start 
    std::string s;
    std::cout << "Enter any key to start" << std::endl;
    std::cin >> s; 
    //유도 시작 -> flight time 설정 
    time_start_ = Clock::now(); 
    guidance_controller_.setFlightStart(time_start_);
    
    //타겟 갱신 시작 
    simulation_runner_.StartSimulation(time_start_);
    //유도 루프 시작 
    guidance_controller_.startGuidanceTask();
    guidance_controller_.join();

    std::cout << "유도 종료" << std::endl;
}