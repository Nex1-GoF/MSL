//DataLinkManager.hpp
#pragma once
#include "TargetStateManager.hpp"
#include "MissileStateManager.hpp"
#include "HeaderPacket.hpp"
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
    char id[5]{};
    char role[10]{};
    char ip[16]{};
    int port{-1};
};

class DataLinkManager {
public:
    DataLinkManager(TargetStateManager& tsm, MissileStateManager & msm)
    :tsm_(tsm), msm_(msm)
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
private:
    TargetStateManager& tsm_;
    MissileStateManager& msm_;
    //콜백 관련 
    Callback termination_callback_;
    //스레드 관련 
    std::atomic<bool> running_{false};
    std::thread datalink_worker_;
    std::thread command_worker_;
    //소켓 관련
    static constexpr int MAX_EVENTS = 10;
    static constexpr int MAXLINE = 1024;
    //유도탄 소켓 생성 데이터 
    std::vector<SocketConfig> config_vector_ = {
                                                SocketConfig{"M001","tx","127.0.0.1", 9010},
                                                SocketConfig{"M001", "msl_com","127.0.0.1", 9012},
                                                SocketConfig{"M001", "tgt_info","127.0.0.1", 9013 }
                                                };
    //fd 저장 맵 
    std::unordered_map<std::string,int> fds_; // {role, fd}
    //송신 버퍼 
    int tx_fd_;
    //패킷 관련
    const char* s_id_ = "M001";
    const char* d_id_ = "C001";
    //수신 측 ip, port
    char dest_ip_[16] = "127.0.0.1";
    int dest_port_ = 8001;

    

    
};