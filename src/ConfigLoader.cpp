// ConfigLoader.cpp
#include "ConfigLoader.hpp"
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace {
    void trim(std::string& s) {
        const char* ws = " \t\r\n";
        auto b = s.find_first_not_of(ws);
        if (b == std::string::npos) { s.clear(); return; }
        auto e = s.find_last_not_of(ws);
        s = s.substr(b, e - b + 1);
    }

    std::uint16_t to_u16(const std::string& v) {
        int x = std::stoi(v);
        if (x < 0 || x > 65535) {
            throw std::runtime_error("port out of range: " + v);
        }
        return static_cast<std::uint16_t>(x);
    }
}

SystemConfig loadSystemConfig(const std::string& path)
{
    SystemConfig cfg; // Vm, guidance_cfg, network_cfg 전부 기본값으로 시작

    std::ifstream ifs(path);
    if (!ifs) {
        throw std::runtime_error("failed to open config file: " + path);
    }

    std::string line;
    std::string section;

    while (std::getline(ifs, line)) {
        trim(line);
        if (line.empty()) continue;
        if (line[0] == '#' || line[0] == ';') continue;

        // 섹션
        if (line.front() == '[' && line.back() == ']') {
            section = line.substr(1, line.size() - 2);
            trim(section);
            continue;
        }

        // key=value
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string val = line.substr(pos + 1);
        trim(key);
        trim(val);

        if (section == "guidance") {
            auto& g = cfg.guidance_cfg;
            if      (key == "distance_terminal") g.distance_terminal = std::stod(val);
            else if (key == "distance_detonate") g.distance_detonate = std::stod(val);
            else if (key == "K_ZEM")             g.K_ZEM             = std::stod(val);
            else if (key == "N_fal")             g.N_fal             = std::stod(val);
            else if (key == "N_near")            g.N_near            = std::stod(val);
            else if (key == "N0")                g.N0                = std::stod(val);
            else if (key == "Kn")                g.Kn                = std::stod(val);
            else if (key == "tau")               g.tau               = std::stod(val);
            else if (key == "Vm")                cfg.Vm              = std::stod(val); // NEW: Vm
        }
        else if (section == "network") {
            auto& n = cfg.network_cfg;
            if      (key == "my_id")             n.my_id            = val;
            else if (key == "c2_id")             n.c2_id            = val;
            else if (key == "c2_ip")             n.c2_ip            = val;
            else if (key == "c2_port")           n.c2_port          = to_u16(val);
            else if (key == "my_ip_rx")          n.my_ip_rx         = val;
            else if (key == "my_port_rx")        n.my_port_rx       = to_u16(val);
            else if (key == "my_ip_tx")          n.my_ip_tx         = val;
            else if (key == "my_port_tx")        n.my_port_tx       = to_u16(val);
            else if (key == "my_ip_tgt_info")    n.my_ip_tgt_info   = val;
            else if (key == "my_port_tgt_info")  n.my_port_tgt_info = to_u16(val);
            else if (key == "my_ip_com_info")    n.my_ip_com_info   = val;
            else if (key == "my_port_com_info")  n.my_port_com_info = to_u16(val);
            else if (key == "rdl_ip")            n.rdl_ip           = val;
            else if (key == "rdl_port")          n.rdl_port         = to_u16(val);
        }
    }

    return cfg;
}
