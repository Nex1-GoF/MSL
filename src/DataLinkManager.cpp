// DataLinkManager.cpp
#include "DataLinkManager.hpp"
#include "TgtInfoPacket.hpp"
#include "MslInfoPacket.hpp"
#include "MslCmdPacket.hpp"


DataLinkManager::~DataLinkManager() noexcept
{
    stopDataLink();

    for (auto &kv : fds_)
    {
        int &fd = kv.second;
        if (fd >= 0)
        {
            ::close(fd);
            fd = -1;
        }
    }
    fds_.clear();
}

void DataLinkManager::setTerminationCallback(Callback cb) {
    termination_callback_ = cb;
}

void DataLinkManager::setDataLink()
{
    for (const auto &cfg : config_vector_)
    {

        int fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd < 0)
        {
            perror("socket creation failed");
            exit(EXIT_FAILURE);
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(cfg.port);
        inet_pton(AF_INET, cfg.ip, &addr.sin_addr);

        if (bind(fd, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)) < 0)
        {
            perror("bind failed");
            close(fd);
            exit(EXIT_FAILURE);
        }
        fds_[cfg.role] = fd; // {role, fd} 맵에 저장
    }
    tx_fd_ = fds_.at("tx"); // 송신 전용 소켓 fd 따로 관리
}

void DataLinkManager::startDataLink()
{
    if (running_.exchange(true))
        return;
    datalink_worker_ = std::thread(&DataLinkManager::DataLinkTask, this); // DataLinkTask 실행
    command_worker_ = std::thread(&DataLinkManager::CommandTask, this);   // CommandTask 실행
}

void DataLinkManager::stopDataLink()
{
    // running_ = false
    if (!running_.exchange(false))
        return;

    // shutdown socket (recvfrom 즉시 return)
    for (auto &kv : fds_)
    {
        int fd = kv.second;
        if (kv.first == "msl_com" || kv.first == "tgt_info")
        {
            ::shutdown(fd, SHUT_RD);
        }
    }

    if (datalink_worker_.joinable())
        datalink_worker_.join();
    if (command_worker_.joinable())
        command_worker_.join();
}

void DataLinkManager::joinDataLink()
{
    if (datalink_worker_.joinable())
        datalink_worker_.join();
    if (command_worker_.joinable())
        command_worker_.join();
}

// 업링크/다운링크 처리 태스크
void DataLinkManager::DataLinkTask()
{

    const int curfd = fds_.at("tgt_info"); // 표적 정보 수신 소켓 fd 

    while (running_)
    {
        uint8_t buffer[MAXLINE];
        sockaddr_in clientAddr{};
        socklen_t len = sizeof(clientAddr);
        // 수신 대기
        int recvsize = recvfrom(curfd, buffer, MAXLINE, 0, (sockaddr *)&clientAddr, &len);

        if (recvsize > 0)
        {
            std::vector<uint8_t> packetData(buffer, buffer + recvsize);
            // end 라는 종료 토큰받으면 수신 태스크 종료
            if (recvsize >= 3 && std::memcmp(buffer, "end", 3) == 0)
                return;
            // handler.handlePacket(packetData, curfd, clientAddr, len);
            if (packetData.size() < HEADER_PACKET_SIZE)
                continue;

            /*1. 최신 타겟 정보 갱신 */
            // (1) deserialize
            TgtInfoPacket pkt = TgtInfoPacket::deserialize(packetData);

            /*----------로깅용---------- */
            pkt.print();
            /*----------로깅용---------- */
            // (2) target_state_t  load
            target_state_t received_tg = pkt.getTargetState();
            // (3) update 
            tsm_.updateState(received_tg.r_t, received_tg.v_t, received_tg.t);

            /* 2. 다운링크 전송(최신 유도탄 정보) */
            missile_state_t msl_to_send = msm_.getMissileState();
            Vec3 r_m = msl_to_send.r_m;
            
            //(1) 헤더 생성
            HeaderPacket header(s_id_, d_id_, 0, MSL_INFO_PACKET_SIZE);
            
            //(2) serialize (임시)
            MslInfoPacket mpk(header, doubleToI32(r_m[0]), doubleToI32(r_m[1]),doubleToI32(r_m[2]),
                                0, 0, 0,
                        doubleToI32(msl_to_send.last_update_time), msl_to_send.f_status, msl_to_send.t_status);
            
            /*----------로깅용---------- */
            mpk.print();
            /*----------로깅용---------- */
            std::vector<uint8_t> packet = mpk.serialize();
            
            //(3) send downlink
            sockaddr_in destAddr{};
            destAddr.sin_family = AF_INET;
            destAddr.sin_port = htons(dest_port_);
            inet_pton(AF_INET, dest_ip_, &destAddr.sin_addr);

            int sent = sendto(tx_fd_, packet.data(), packet.size(), 0,
                              (sockaddr *)&destAddr, sizeof(destAddr));

            if (sent < 0)
                perror("sendto");
                
        }
        else if (recvsize == 0)
        {
            // UDP에선 드물지만, 읽기 종료로 깨어난 경우 방어적으로 처리
            if (!running_)
                break; // 정상 종료 경로
            continue;  // 비정상 0이라면 다음 루프로
        }
        else
        { // recvsize == -1
            if (errno == EINTR)
            {
                // 시그널 개입: 깔끔히 재시도
                continue;
            }
        }
   
    }
}
// 비상 폭파 명령 수신, 처리 태스크
// 우선 순위 설정도 고려
void DataLinkManager::CommandTask()
{
    const int curfd = fds_.at("msl_com"); // 비상 폭파

    while (running_)
    {
        uint8_t buffer[MAXLINE];
        sockaddr_in clientAddr{};
        socklen_t len = sizeof(clientAddr);
        // 수신 대기
        int recvsize = recvfrom(curfd, buffer, MAXLINE, 0, (sockaddr *)&clientAddr, &len);
        if (recvsize > 0)
        {
            /*수신 즉시 비상 폭파 명령 처리 (패킷 깔 필요 x)*/
            // (1). 종료 로그 저장 msm_.updateAFinalLog()
            // (2)  stopDataLink , stopGuidanceTask
            if (termination_callback_) termination_callback_(); // TaskManager.stop() 호출 -> 모든 태스크 종료(guidance, datalink, cmd) 

        }
        else if (recvsize == 0)
        {
            // UDP에선 드물지만, 읽기 종료로 깨어난 경우 방어적으로 처리
            if (!running_)
                break; // 정상 종료 경로
            continue;  // 비정상 0이라면 다음 루프로
        }
        else
        { // recvsize == -1
            if (errno == EINTR)
            {
                // 시그널 개입: 깔끔히 재시도
                continue;
            }
        }
    }
}
