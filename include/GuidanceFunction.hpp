#pragma once
#include "Utils.hpp"               // Vec3, add3/sub3/... 등
#include "MissileStateManager.hpp" // missile_state_t
#include "TargetStateManager.hpp"  // target_state_t

class GuidanceFunction {
public:
    // CV 표적 요격 해(값 반환용이므로 '완전 정의'가 헤더에 있어야 함)
    struct InterceptSolution {
        Vec3   pip{ 0,0,0 };  // Predicted Intercept Point
        double tgo{ 0.0 };    // time-to-go
        bool   ok{ false };   // 근 양의 해 존재 여부
    };

protected:
    InterceptSolution solveInterceptCV(const missile_state_t& m,
        const target_state_t& t) const;

    Vec3 runAutopilot(Vec3 a_cmd,
        const missile_state_t& m,
        const target_state_t& t,
        double dt);
};
