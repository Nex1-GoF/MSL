// Config.hpp
#pragma once          
#include <string>
#include <cstdint>

struct GuidanceConfig{
    double distance_terminal  = 10000.0;
    double distance_detonate  = 1000.0;
    double K_ZEM              = 0.0;
    double N_fal              = 0.0;
    double N_near             = 0.0;
    double N0                 = 0.0;
    double Kn                 = 0.0;
    double tau                = 0.0;
};

struct NetworkConfig{
    /*ID*/
    std::string my_id; //"M001" ~ "M004"
    std::string c2_id; //"C001"
    
    /*발사절차 관련 (C2와 직접 통신)*/
    std::string c2_ip;     // 발사 절차 송신용  
    std::uint16_t c2_port = 0;

    std::string my_ip_rx;  // 발사 절차 응답용
    std::uint16_t my_port_rx = 0; 

    /*데이터링크 관련(RDL과 통신)*/
    std::string my_ip_tx;
    std::uint16_t my_port_tx = 0;

    std::string my_ip_tgt_info;
    std::uint16_t my_port_tgt_info = 0;
    
    std::string my_ip_com_info;
    std::uint16_t my_port_com_info = 0;

    std::string rdl_ip;     // dest_ip 
    std::uint16_t rdl_port = 0;  // dest_port
};

struct SystemConfig {
    double Vm = 0.0;
    GuidanceConfig guidance_cfg;
    NetworkConfig  network_cfg;
};
