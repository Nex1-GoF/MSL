#include "SystemContext.hpp"


SystemContext::SystemContext()
    :time_start_{},                 // 1) 먼저 초기화
    initial_r_m{0,0,0},
    initial_u_m{1,0,0},
    msm_{/* 필요 시 초기값 */},       // 2) 
    tsm_{/* 필요 시 초기값 */},       // 3)
    guidance_controller_{msm_, tsm_, time_start_} // 4) 마지막: 참조 주입
{
   
}

//메인 스레드 
void
SystemContext::run() {
    running_ = true;
    while(running_) { 
        toIdle_(); //소켓등 시스템 자원 초기화
        if(!runLaunchProcedure_()) continue; //발사 절차->중단 시 발사 대기 상태 재진입 
        startInitialGuidance(); //초기 유도 수행
        startDataLink_(); //통신 관련 태스크 실행 
        startGuidance_(); //유도 태스크 실행 -> 상태관리, 전략 설정은 GuidanceContoller 가 담당 
        waitMissionEnd_(); //종료 이벤트까지 대기 
        finalizeAndReset_(); // 종료 절차 
    }
}

void 
SystemContext::toIdle_() {
    //1.시스템 자원 초기화 
    //2.소켓 설정
    //3. ,,, 
}


bool 
SystemContext::runLaunchProcedure_() {

    //1. 발사 절차 (핑퐁)  ()
    //2. 발사 명령 + 초기 포착 지점 수신 이벤트 
    //  "초기 포작 지점"과 "유도탄 초기 위치"(rm, 발사대 위치)로 유도탄 초기 방향(um) 계산 
    //  SystemContext의 time_start_ 초기화 (시간 동기화용 -> real time을 flight time으로 변환할 때 기준 시각)
    time_start_ = Clock::now(); 
    guidance_controller_.setFlightStart(time_start_);
    //3. 유도탄 정보 초기화 (0초의 유도탄 정보) -> MissileStateManager ->update(rm,um,0)메세지 보내기. 
    //4. return 
    return true;
}

void startInitialGuidance() {
    // 0초의 유도탄 정보로 5초의 유도탄 정보 계산 후 초기화-> MissileStateManager ->update(rm,um,5)메세지 보내기. 
    // 5초 후 return 
}

void 
SystemContext::startDataLink_() {
    //1. 수신 스레드 start -> Receiver 객체에게 start() 메세지 전송 
    //  receiver_.start();
    //2. 수신 처리 스레드 start -> ReceiverProcessor 객체에게 start() 메세지 전송
    //  receive_processor_.start();
    //외부로부터 주기적으로 최신 표적 정보 수신받아 update 시작
    //return; 
}
void 
SystemContext::startGuidance_() {
    //1. guidance controller 객체 초기 상태 MID로 초기화 
    //2. 유도 태스크 start -> guindance controller 객체에 start() 메세지 전송
    guidance_controller_.start();
}

void 
SystemContext::waitMissionEnd_() {
    //종료 이벤트 대기
    guidance_controller_.join();
    //종료 이벤트 발생하면 return
}

void 
SystemContext::finalizeAndReset_() {
    //종료 시점 유도탄 정보, flight time 전송 
}