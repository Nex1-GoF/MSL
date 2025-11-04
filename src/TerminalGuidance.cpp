//TerminalGuidance.cpp
#include "IGuidance.hpp"
#include "TerminalGuidance.hpp"
#include <algorithm>

static inline double clip(double x, double a, double b) { return x < a ? a : (x > b ? b : x); }
Vec3
TerminalGuidance::calculateGuidance(missile_state_t m, target_state_t t, double dt) {
    const int PN_SIGN = +1;
    const double N0 = 3.0;   
    const double kN = 2.5;     
    const double Kz = 0.25;    // ZEM 계수

    const Vec3 R = sub3(t.r_t, m.r_m);
    const double Rmag = norm3(R);
    const Vec3 rhat = (Rmag < 1e-9) ? Vec3{ 0,0,0 } : scale3(R, 1.0 / Rmag);

    const Vec3 v_m = scale3(m.u_m, m.V_m);
    const Vec3 v_rel = sub3(t.v_t, v_m);

    // t_go 대략치 (인터셉트 해가 있으면 그걸 사용)
    double tgo = 5.0;
    {
        auto sol = solveInterceptCV(m, t);
        if (sol.ok) tgo = sol.tgo;
        else {
            const double Vc = std::max(-dot3(v_rel, rhat), 50.0);
            tgo = clip(Rmag / Vc, dt, 30.0);
        }
    }

    // LOS rate 벡터(표적 LOS 기준)
    const double R2 = std::max(dot3(R, R), 1e-9);
    const Vec3 omega = scale3(cross3(R, v_rel), 1.0 / R2);

    // 폐쇄속도(양수)
    const double Vc_tar = std::max(-dot3(v_rel, rhat), 5.0);

    // 게인: t_go가 줄수록 커짐
    const double N_eff = N0 + kN / std::max(0.2, tgo);

    // PN 가속(3D 벡터형): a = N * Vc * (ω × r̂)
    Vec3 a_pn = scale3(cross3(omega, rhat), PN_SIGN * N_eff * Vc_tar);

    // 작은 ZEM 보강: ZEMt = (r_t - r_m) + (v_t - v_m)*t_go
    const Vec3 ZEMt = add3(sub3(t.r_t, m.r_m), scale3(sub3(t.v_t, v_m), tgo));
    Vec3 a_z = (tgo > dt) ? scale3(ZEMt, Kz / (tgo * tgo)) : Vec3{ 0,0,0 };

    Vec3 a_cmd = add3(a_pn, a_z);

    // 기수방향 성분 제거 → 횡가속만
    const double along = dot3(a_cmd, m.u_m);
    a_cmd = sub3(a_cmd, scale3(m.u_m, along));
    return runAutopilot(a_cmd, m, t, dt);
}