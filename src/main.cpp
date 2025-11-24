#include<algorithm>
#include <mutex>
#include <array>
#include <chrono>
#include <optional>
#include <cmath>
#include "SystemContext.hpp"
#include "ConfigLoader.hpp"


int main(int argc, char** argv)
{
    std::string cfgPath = "config.txt"; //default

    if (argc > 1) {
        // argv[1]은 "1", "2" 같은 문자열
        std::string argNum = argv[1];

        // 문자열을 조합하여 파일명 완성
        // 예: "1"이 들어오면 -> "config_" + "1" + ".txt" = "config_1.txt"
        cfgPath = "config_" + argNum + ".txt";
    }
    
     SystemConfig cfg;
       try {
        cfg = loadSystemConfig(cfgPath);
    } catch (const std::exception& e) {
        std::cerr << "Config load error: " << e.what() << std::endl;
        return 1;
    }

    while(true){
        
        SystemContext sys_context(cfg);
        sys_context.run();
    }
    
    return 0;
}