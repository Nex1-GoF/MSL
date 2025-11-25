//MidtermGuidance.cpp
#include "IGuidance.hpp"
#include "MidtermGuidance.hpp"
#include <algorithm>

Vec3
MidtermGuidance::calculateGuidance(missile_state_t m,
                                   target_state_t t,
                                   double dt)
{
    // [MATCH MATLAB] 파라미터
    constexpr int   PN_SIGN    = +1;
    constexpr double N_far     = 4.0;
    constexpr double N_near    = 10.0;
    constexpr double K_ZEM     = 0.5;
    constexpr double R_GAIN_FAR  = 1200.0;
    constexpr double R_GAIN_NEAR = 300.0;
    constexpr double tgo_far   = 10.0;
    constexpr double tgo_near  = 2.0;

    // ----- 기본 기하 (실제 표적 기준) -----
    const Vec3 R_tar = sub3(t.r_t, m.r_m);
    const double Rmag = norm3(R_tar);
    const Vec3 v_m    = scale3(m.u_m, m.V_m);
    const Vec3 v_rel  = sub3(t.v_t, v_m);

    // ----- t_go, PIP 계산 -----
    InterceptSolution sol = solveInterceptCV(m, t);
    double tgo = sol.tgo;
    Vec3   r_pip = sol.pip;

    // ----- PIP / Target 블렌딩 weight -----
    double w_pip = 1.0; // 초기값 아무거나

// w_num == 0 에 해당 (blend)
    if (tgo_far > tgo_near + 1e-9) {
        double w = (tgo - tgo_near) / (tgo_far - tgo_near);
        w_pip = clip(w, 0.0, 1.0);   // 0~1로 클립
    } else {
        w_pip = 1.0;
    }

    // r_ref = w_pip * r_pip + (1 - w_pip) * t.r_t
    Vec3 r_ref = add3(scale3(r_pip, w_pip),
                      scale3(t.r_t, (1.0 - w_pip)));

    // ----- PN용 LOS 기하 (r_ref 기준) -----
    Vec3  R_ref = sub3(r_ref, m.r_m);
    double R2   = std::max(dot3(R_ref, R_ref), 1e-9);
    Vec3  rhat  = scale3(R_ref, 1.0 / (std::sqrt(R2) + 1e-9));
    Vec3  omega = scale3(cross3(R_ref, v_rel), 1.0 / R2);

    // [MATCH MATLAB] Vc 하한 1.0
    double Vc   = std::max(-dot3(v_rel, rhat), 1.0);

    // ----- 게인 스케줄 (tgo, Range) -----
    double phi_tgo = 0.0;
    if (tgo_far > tgo_near + 1e-9) {
        phi_tgo = (tgo - tgo_near) / (tgo_far - tgo_near);
        phi_tgo = clip(phi_tgo, 0.0, 1.0);
    }
    double N_eff_t = N_near - (N_near - N_far) * phi_tgo;

    double phi_rg  = 0.0;
    if (R_GAIN_FAR > R_GAIN_NEAR + 1e-9) {
        phi_rg  = (R_GAIN_FAR - Rmag) / (R_GAIN_FAR - R_GAIN_NEAR);
        phi_rg  = clip(phi_rg, 0.0, 1.0);
    }
    double N_eff_r = N_far + (N_near - N_far) * phi_rg;

    constexpr double w_tgo = 0.6;
    double N_eff = w_tgo * N_eff_t + (1.0 - w_tgo) * N_eff_r;

    // ----- PN + ZEM ----- (ZEM은 현재 비활성화)
    Vec3 a_pn = scale3(cross3(omega, rhat), PN_SIGN * N_eff * Vc);

    // MATLAB과 동일하게 ZEM은 계산만 해두고 기본은 끔
    Vec3 v_tar_eff = scale3(t.v_t, (1.0 - w_pip));
    Vec3 ZEM       = add3(sub3(r_ref, m.r_m),
                          scale3(sub3(v_tar_eff, v_m), tgo));
    Vec3 a_z{0,0,0};
    if (tgo > dt) {
        a_z = scale3(ZEM, K_ZEM / (tgo * tgo));
    }

    Vec3 a_cmd = a_pn;          // ★ MATLAB: a_pn 만 사용 (ZEM off)
    // Vec3 a_cmd = add3(a_pn, a_z); // ZEM 켜고 싶으면 둘 다에서 동시에 켜기

    // 여기서 횡가속 강제 제거는 굳이 안 해도 됨.
    // MATLAB에서는 runAutopilot 안에서만 횡가속으로 투영하니까
    // C++도 runAutopilot에 그대로 넘겨주는 게 더 동일함.

    return runAutopilot(a_cmd, m, t, dt);
}

