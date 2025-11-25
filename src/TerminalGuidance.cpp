// TerminalGuidance.cpp
#include "IGuidance.hpp"
#include "TerminalGuidance.hpp"
#include <algorithm>

Vec3
TerminalGuidance::calculateGuidance(missile_state_t m, target_state_t t, double dt)
{
    const int    PN_SIGN = +1;
    const double N0      = 3.0;
    const double kN      = 2.0;
    const double Kz      = 0.3;

    // --- 기본 기하 ---
    const Vec3 R     = sub3(t.r_t, m.r_m);
    const double Rmag = norm3(R);
    const Vec3 rhat   = (Rmag < 1e-9) ? Vec3{0,0,0} : scale3(R, 1.0 / Rmag);

    const Vec3 v_m   = scale3(m.u_m, m.V_m);
    const Vec3 v_rel = sub3(t.v_t, v_m);

    // --- t_go 계산 (MATLAB solveInterceptCV 동일 로직) ---
    double tgo = 5.0;
    {
        auto sol = solveInterceptCV(m, t);
        if (sol.ok) {
            tgo = sol.tgo;
        } else {
            double Vc_rt = -dot3(v_rel, rhat);
            if (Vc_rt > 1e-3) tgo = std::max(dt, std::min(Rmag / Vc_rt, 30.0));
            else tgo = 2.5;
        }
    }

    // ========================================
    // 오버어택 제거 → aim_t = t.r_t
    // ========================================
    Vec3 aim_t = t.r_t;   // 그대로 타깃 조준

    // --- aim LOS 기하 ---
    const Vec3 R_aim = sub3(aim_t, m.r_m);
    const double R2  = std::max(dot3(R_aim, R_aim), 1e-9);
    const Vec3 rhat_aim =
        (R2 < 1e-9) ? Vec3{0,0,0} : scale3(R_aim, 1.0 / std::sqrt(R2));

    // --- LOS rate ---
    const Vec3 omega = scale3(cross3(R_aim, v_rel), 1.0 / R2);

    // --- 폐쇄속도 (MATLAB 동일) ---
    const double Vc = std::max(-dot3(v_rel, rhat_aim), 0.0);

    // --- Navigation gain ---
    const double N_eff = N0 + kN / std::max(0.2, tgo);

    // --- PN ---
    Vec3 a_pn = scale3(cross3(omega, rhat_aim), PN_SIGN * N_eff * Vc);

    // --- ZEM (aim_t 기준, MATLAB 동일) ---
    const Vec3 ZEMt = add3(sub3(aim_t, m.r_m), scale3(sub3(t.v_t, v_m), tgo));
    Vec3 a_z = (tgo > dt)
        ? scale3(ZEMt, Kz / (tgo * tgo))
        : Vec3{0,0,0};

    Vec3 a_cmd = add3(a_pn, a_z);

    // --- 횡가속만 ---
    const double along = dot3(a_cmd, m.u_m);
    a_cmd = sub3(a_cmd, scale3(m.u_m, along));

    // --- autopilot ---
    return runAutopilot(a_cmd, m, t, dt);
}
