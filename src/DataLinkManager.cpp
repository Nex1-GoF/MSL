// DataLinkManager.cpp
#include "DataLinkManager.hpp"
#include <unistd.h>
#include <arpa/inet.h>

DataLinkManager::~DataLinkManager() noexcept
{

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

void DataLinkManager::setDataLink()
{
    for (auto cfg : config_vector_)
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
    if (!running_.exchange(false))
        return;
    if (datalink_worker_.joinable())
        datalink_worker_.join();
    if (command_worker_.joinable())
        command_worker_.join();
}

// 업링크/다운링크 처리 태스크
void DataLinkManager::DataLinkTask()
{
    const int curfd = fds_.at("tgt_info"); // 표적 정보
    uint8_t buffer[MAXLINE];
    sockaddr_in clientAddr{};
    socklen_t len = sizeof(clientAddr);
    
    while (running_)
    {
        int recvsize = recvfrom(curfd, buffer, MAXLINE, 0, (sockaddr *)&clientAddr, &len);
        if (recvsize > 0)
        {
            std::vector<uint8_t> packetData(buffer, buffer + recvsize);
            
            if (recvsize >= 3 && std::memcmp(buffer, "end", 3) == 0)
                return;

            //handler.handlePacket(packetData, curfd, clientAddr, len);
            if (packetData.size() < HeaderPacket::SIZE) return;
            HeaderPacket header = HeaderPacket::deserialize(packetData);
            std::vector<uint8_t> payload(packetData.begin() + HeaderPacket::SIZE, packetData.end());
            
            /*
                구현 예정 
                payload 추출 -> TargetStateManager에 업데이트 -> downlink로 missile_state_t cur = msm_.getCurrentMissile(time_now) 후 패킷 구성 -> send to RDL
            */
        }
        else if (recvsize == -1 &&
                 (errno == EAGAIN || errno == EWOULDBLOCK)) //수정 필요 (블로킹에 맞게)
        {
            break;
        }
        else
        {
            perror("recvfrom");
            break;
        }
        
    }
}
// 비상 폭파 명령 수신, 처리 태스크
void DataLinkManager::CommandTask()
{
    const int fd = fds_.at("msl_com"); // 비상 폭파
    while (running_)
    {
        /* 구현 예정 */
    }
}
