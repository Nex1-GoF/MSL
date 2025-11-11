#include "SystemContext.hpp"
#include <iostream>

// 생성자 구현
SystemContext::SystemContext()
    : time_start_{}, // flight_time 기준 시각
      msm_{},
      tsm_{},
      simulation_runner_{tsm_},
      datalink_manager_{tsm_, msm_},
      guidance_controller_{msm_, tsm_, time_start_},
      task_manager_{guidance_controller_, datalink_manager_} {};

// 메인 스레드
void SystemContext::run()
{
    running_ = true;
    while (running_)
    {
        std::cout << "wati to launch" << '\n';
        toIdle_(); // 소켓등 시스템 자원 초기화
        if (!runLaunchProcedure_())
            continue;           // 발사 절차->중단 시 발사 대기 상태 재진입
        //startInitialGuidance(); // 초기 유도 수행
        //startGuidance_();       // 유도 태스크 실행 -> 상태관리, 전략 설정은 GuidanceContoller 가 담당
       // waitMissionEnd_();      // 종료 이벤트까지 대기
        //finalizeAndReset_();    // 종료 절차
        // runSimulation_();
    }
}
/*---------------------------------------------------------------------------------------*/
void SystemContext::toIdle_()
{
    // 1. 멤버 객체 초기화
    // 2. 발사 절차를 위한 소켓 설정

    // 수신 소켓
    int fd_rx = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_rx < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    sockaddr_in addr_rx{};
    addr_rx.sin_family = AF_INET;
    addr_rx.sin_port = htons(port_rx_);
    inet_pton(AF_INET, ip_rx_, &addr_rx.sin_addr);

    if (bind(fd_rx, reinterpret_cast<const sockaddr *>(&addr_rx), sizeof(addr_rx)) < 0)
    {
        perror("bind failed");
        close(fd_rx);
        exit(EXIT_FAILURE);
    }
    fd_rx_ = fd_rx;

    // 송신 소켓
    int fd_tx = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_tx < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    sockaddr_in addr_tx{};
    addr_tx.sin_family = AF_INET;
    addr_tx.sin_port = htons(port_tx_);
    inet_pton(AF_INET, ip_tx_, &addr_tx.sin_addr);

    if (bind(fd_tx, reinterpret_cast<const sockaddr *>(&addr_tx), sizeof(addr_tx)) < 0)
    {
        perror("bind failed");
        close(fd_tx);
        exit(EXIT_FAILURE);
    }
    fd_tx_ = fd_tx;
}

bool SystemContext::runLaunchProcedure_()
{

    int32_t pip_x, pip_y, pip_z;
    uint8_t key[32];

    while (true)
    {

        // recvfrom(buffer)
        
        uint8_t buffer[MAXLINE];
        sockaddr_in clientAddr{};
        socklen_t len = sizeof(clientAddr);
        // 수신 대기
        int recvsize = recvfrom(fd_rx_, buffer, MAXLINE, 0, (sockaddr *)&clientAddr, &len);
        if (recvsize > 0)
        {
            std::vector<uint8_t> packetData(buffer, buffer + recvsize);

            // seq 추출
            uint32_t cur_seq =
                (uint32_t(buffer[8]) << 24) |
                (uint32_t(buffer[9]) << 16) |
                (uint32_t(buffer[10]) << 8) |
                uint32_t(buffer[11]);

            if (cur_seq == 4)
            {
                // 세션 키 수신
                KeyPacket rcv_pk = KeyPacket::deserialize(packetData);
                rcv_pk.getKey(key);
                // 로그
                std::cout << "RX cur_seq=" << cur_seq << " key=";
                for (int i = 0; i < 32; ++i)
                {
                    if (i) std::cout << ' ';
                    std::cout << (int)key[i];
                }
                std::cout << "\n";
            }
            else if (cur_seq == 6)
            {
                // pip 수신
                PipPacket rcv_pk = PipPacket::deserialize(packetData);
                pip_x = rcv_pk.getX(); pip_y = rcv_pk.getY(); pip_z = rcv_pk.getZ();
                // 로그
                 std::cout << "RX cur_seq=" << cur_seq
                << " pip=(" << pip_x << "," << pip_y << "," << pip_z << ")\n";
            }
            else if (cur_seq == 8)
            {
                // 취소 명령 수신
                cur_seq = 0;
                // key[32] 초기화
                std::memset(key, 0, 32);
                close(fd_rx_); fd_rx_ = -1;
                close(fd_tx_); fd_tx_ = -1;
                return false;
            }
            else
            {
                //로그
                std::cout << "RX cur_seq=" << cur_seq << "\n";
            }

            // 응답 메세지 전송 (8번 패킷 외 공통 )
            HeaderPacket header(s_id_, d_id_, cur_seq, HEADER_PACKET_SIZE);
            std::vector<uint8_t> packet = header.serialize();

            sockaddr_in destAddr{};
            destAddr.sin_family = AF_INET;
            destAddr.sin_port = htons(dest_port_);
            inet_pton(AF_INET, dest_ip_, &destAddr.sin_addr);

            int sent = sendto(fd_tx_, packet.data(), packet.size(), 0,
                              (sockaddr *)&destAddr, sizeof(destAddr));

            if (sent < 0)
                perror("sendto");

            // 모든 절차 완료
            if (cur_seq == 7)
            {
                close(fd_rx_); fd_rx_ = -1;
                close(fd_tx_); fd_tx_ = -1;
                break;
            }
        }
    }

    // //"초기 포작 지점"과 "유도탄 초기 위치"(rm, 발사대 위치)로 유도탄 초기 방향(um) 계산(등속 직선 운동을 위해)
    // Vec3 u_m_init = getInitialPIP_();
    // msm_.setInitialState(u_m_init);
    // //  SystemContext의 time_start_ 초기화 (시간 동기화용 -> real time을 flight time으로 변환할 때 기준 시각)
    // time_start_ = Clock::now();
    // // 유도 태스크 실행 객체의 기준 시간 초기화
    // guidance_controller_.setFlightStart(time_start_);
    std::cout << "complete launch procedure" << std::endl;
    return true;
}

void SystemContext::startInitialGuidance()
{
    /*확정 x*/
    while (true)
    {

        TimePoint real_time_now = Clock::now();
        double flight_time_now = std::chrono::duration_cast<Duration>(real_time_now - time_start_).count();
        if (flight_time_now >= 5.0)
        {
            msm_.updateForInitialGuidance(flight_time_now); // 현재 시각 기준으로 msl 업데이트 (등속직선운동 용)
            break;
        }
        /* 구현 x -100 ms 동안 sleep" */
        std::this_thread::sleep_until(Clock::now() + 200ms);
    }

    return;
}

void SystemContext::startGuidance_()
{
    task_manager_.start(); // 모든 태스크 한꺼번에 실행 (유도, 데이터링크, 비상 폭파 명령 수신)
    std::cout << "start datalink(wait...)" << std::endl;

    return;
}

void SystemContext::waitMissionEnd_()
{

    task_manager_.join(); // 종료이벤트 발생까지 대기
    return;
}

void SystemContext::finalizeAndReset_()
{
    // 종료 시점 유도탄 정보, flight time 전송 (GuidanceController가 종료시점의 상태 업데이트 역할)
    // 여기서는 그냥 불러와서 전송만
    /*통신부 완료 후 구현 예정 */
}
/*---------------------------------------------------------------------------------------*/
/*
Vec3
SystemContext::getInitialPIP_() {
    //수신한 초기 표적 정보, 유도탄 초기 위치로 u_m_initial 계산 후 return

}
*/
void SystemContext::runSimulation_()
{
    // 표적 초기 상태 설정
    Vec3 r_t_initial = {12000.0, 12000.0, 22000.0}; // 초기 위치 (0초에서의 위치)
    Vec3 v_t_initial = {-600.0, -600.0, 0};         // 등속
    tsm_.updateState(r_t_initial, v_t_initial, 0);
    // wait to start
    std::string s;
    std::cout << "Enter any key to start" << std::endl;
    std::cin >> s;
    // 유도 시작 -> flight time 설정
    time_start_ = Clock::now();
    guidance_controller_.setFlightStart(time_start_);

    // 타겟 갱신 시작
    simulation_runner_.StartSimulation(time_start_);
    // 유도 루프 시작
    guidance_controller_.startGuidanceTask();
    guidance_controller_.join();

    std::cout << "유도 종료" << std::endl;
}