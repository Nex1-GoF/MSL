// GuidanceBase.hpp — 공통 유틸 모음(상태가 필요 없으면 정적/자유함수로도 가능)
#pragma once
#include "Utils.hpp"
#include "MissileStateManager.hpp"
#include "TargetStateManager.hpp"

struct InterceptSolution {
        Vec3   pip{ 0,0,0 };  // Predicted Intercept Point
        double tgo{ 0.0 };    // time-to-go
        bool   ok{ false };   // 근 양의 해 존재 여부
};

class GuidanceFunction {
protected:
    
    //공통 함수 선언 
    double losRate(const missile_state_t& m, const target_state_t& t);

    InterceptSolution solveInterceptCV(const missile_state_t& m, const target_state_t& t) const;
    Vec3 runAutopilot(Vec3 a_cmd, const missile_state_t& m, const target_state_t& t, double dt);

};