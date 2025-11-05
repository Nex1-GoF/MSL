//MidtermGuidance.cpp
#include "IGuidance.hpp"
#include "MidtermGuidance.hpp"
#include <algorithm>

Vec3
MidtermGuidance::calculateGuidance(missile_state_t m, target_state_t t, double dt){
    const int   PN_SIGN = +1;
    const double N_far = 4.0;
    const double N_near = 10.0;
    const double K_ZEM = 0.5;


    const double R_GAIN_FAR = 1200.0, R_GAIN_NEAR = 300.0;
    const double tgo_far = 10.0, tgo_near = 2.0;

    const Vec3 R = sub3(t.r_t, m.r_m);
    const double Rmag = norm3(R);
    const Vec3 rhat = (Rmag < 1e-9) ? Vec3{ 0,0,0 } : scale3(R, 1.0 / Rmag);

    auto sol = solveInterceptCV(m, t);
    double tgo = 0.0; Vec3 r_pip;
    if (sol.ok) { tgo = sol.tgo; r_pip = sol.pip; }
    else {
        // 기하학적 근사: t_go ≈ R/ Vc(LOS)
        const Vec3 v_rel_geom = sub3(t.v_t, scale3(m.u_m, m.V_m));
        const double Vc_geom = std::max(-dot3(v_rel_geom, rhat), 50.0);
        tgo = clip(Rmag / Vc_geom, dt, 30.0);
        r_pip = add3(t.r_t, scale3(t.v_t, tgo));
    }

    const Vec3 Rpip = sub3(r_pip, m.r_m);
    const double Rp = norm3(Rpip);
    const Vec3 rhat_pip = (Rp < 1e-9) ? Vec3{ 0,0,0 } : scale3(Rpip, 1.0 / Rp);

    // 상대속도(현재)
    const Vec3 v_rel = sub3(t.v_t, scale3(m.u_m, m.V_m));

    // LOS rate (벡터형) — ω = (r × v_rel)/|r|^2   (여기선 PIP-LOS 기준)
    const double Rp2 = std::max(dot3(Rpip, Rpip), 1e-9);
    const Vec3 omega = scale3(cross3(Rpip, v_rel), 1.0 / Rp2);

    // 폐쇄속도(양수) — PIP 기준
    const double Vc_pip = std::max(-dot3(v_rel, rhat_pip), 5.0);

    // ===== 2) 게인 스케줄 =====
    const double phi_tgo = clip01((tgo - tgo_near) / (tgo_far - tgo_near)); // far→1, near→0
    const double N_eff_t = N_near - (N_near - N_far) * phi_tgo;

    const double phi_rg = clip01((R_GAIN_FAR - Rmag) / (R_GAIN_FAR - R_GAIN_NEAR)); // far→0, near→1
    const double N_eff_r = N_far + (N_near - N_far) * phi_rg;

    const double w_tgo = 0.6;
    const double N_eff = w_tgo * N_eff_t + (1.0 - w_tgo) * N_eff_r;

    // ===== 3) PN + ZEM =====
    Vec3 a_pn = scale3(cross3(omega, rhat_pip), PN_SIGN * N_eff * Vc_pip);

    // 작은 ZEM 보강
    // ZEM = (r_pip - r_m) + (v_t - v_m)*t_go
    const Vec3 v_m = scale3(m.u_m, m.V_m);
    const Vec3 ZEM = add3(sub3(r_pip, m.r_m), scale3(sub3(t.v_t, v_m), tgo));
    Vec3 a_z = (tgo > dt) ? scale3(ZEM, K_ZEM / (tgo * tgo)) : Vec3{ 0,0,0 };
    // (과도 크기 제한은 상위 제어에서 포화/LPF가 있을 것으로 가정 → 여기선 생략)

    Vec3 a_cmd = add3(a_pn, a_z);

    // 횡가속도
    const double along = dot3(a_cmd, m.u_m);
    a_cmd = sub3(a_cmd, scale3(m.u_m, along));

    return runAutopilot(a_cmd, m, t, dt);

    }
