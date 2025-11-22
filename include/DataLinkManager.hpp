//DataLinkManager.hpp
#pragma once
#include "TargetStateManager.hpp"
#include "MissileStateManager.hpp"
#include "HeaderPacket.hpp"
#include "Config.hpp"
#include <vector>
#include <thread>
#include <functional>
#include <atomic>
#include <unordered_map>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <cstdint>
#include <unistd.h>


struct SocketConfig {
    std::string my_id;
    std::string role;   // "tx", "msl_com", "tgt_info"
    std::string ip;
    uint16_t    port;
};

class DataLinkManager {
public:
    DataLinkManager( TargetStateManager& tsm, MissileStateManager & msm, TimePoint flight_time,const NetworkConfig& net_cfg)
    :tsm_(tsm), msm_(msm), flight_time_(flight_time), net_cfg_(net_cfg),
    config_vector_{
        SocketConfig{net_cfg_.my_id, "tx",       net_cfg_.my_ip_tx,       net_cfg_.my_port_tx},
        SocketConfig{net_cfg_.my_id, "msl_com",  net_cfg_.my_ip_com_info, net_cfg_.my_port_com_info},
        SocketConfig{net_cfg_.my_id, "tgt_info", net_cfg_.my_ip_tgt_info, net_cfg_.my_port_tgt_info}
      }
    {};

    ~DataLinkManager();

    using Callback = std::function<void()>;
    void setTerminationCallback(Callback cb);
    
    void setDataLink();
    void startDataLink();
    void stopDataLink();
    void joinDataLink();
    //스레드 (태스크) 함수
    void DataLinkTask(); //업링크->다운링크 
    void CommandTask();  //비상 폭파 명령 수신, 처리 
    double getFlightTimeNow();
    void setFlightStart(TimePoint tp);
    void sendDownLink();
private:
    TargetStateManager& tsm_;
    MissileStateManager& msm_;
    TimePoint flight_time_{}; //발사 시작 시간(절대 시간)
    const NetworkConfig net_cfg_;
    //콜백 관련 
    Callback termination_callback_;
    //스레드 관련 
    std::atomic<bool> running_{false};
    std::thread datalink_worker_;
    std::thread command_worker_;
    //소켓 관련
    static constexpr int MAX_EVENTS = 10;
    static constexpr int MAXLINE = 1024;
    

    
    //유도탄 소켓 생성 데이터 -통합
    std::vector<SocketConfig> config_vector_ ;
    //fd 저장 맵 
    std::unordered_map<std::string,int> fds_; // {role, fd}
    //송신 버퍼 
    int tx_fd_;
    //패킷 관련
    const char* s_id_ = "M001";
    const char* d_id_ = "C001";
    //수신 측 ip, port
    char dest_ip_[16] = "192.168.1.10";
    int dest_port_ = 8001;

    
};