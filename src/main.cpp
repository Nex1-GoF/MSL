#include<algorithm>
#include <mutex>
#include <array>
#include <chrono>
#include <optional>
#include <cmath>
#include "SystemContext.hpp"



int main() {
    
    while(true){
        SystemContext sys_context;
        sys_context.run();
    }
    return 0;
}