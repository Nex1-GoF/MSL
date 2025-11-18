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
    std::string cfgPath = "config.txt";
    if (argc > 1) {
        cfgPath = argv[1]; // 다른 설정파일 선택 가능
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