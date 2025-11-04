#include "GuidanceFunction.hpp"
#include <cmath>
#include <algorithm>

namespace {
    inline double clip(double x, double a, double b) { return x < a ? a : (x > b ? b : x); }
}

GuidanceFunction::InterceptSolution
GuidanceFunction::solveInterceptCV(const missile_state_t& m,
    const target_state_t& t) const
{
    // 해석적 t_go 해: (v_t·v_t - V_m^2) τ^2 + 2 (R·v_t) τ + (R·R) = 0
    InterceptSolution sol;
    const Vec3 R = sub3(t.r_t, m.r_m);
    const double V_m = m.V_m;
    const double a = dot3(t.v_t, t.v_t) - V_m * V_m;
    const double b = 2.0 * dot3(R, t.v_t);
    const double c = dot3(R, R);

    if (std::fabs(a) < 1e-12) {
        if (b < 0.0) {
            sol.tgo = -c / b;
            sol.ok = (sol.tgo > 0.0);
        }
    }
    else {
        const double D = b * b - 4.0 * a * c;
        if (D >= 0.0) {
            const double rt = std::sqrt(D);
            const double t1 = (-b - rt) / (2.0 * a);
            const double t2 = (-b + rt) / (2.0 * a);
            // 양의 해 중 작은 것
            double tau = 1e100;
            if (t1 > 1e-6) tau = std::min(tau, t1);
            if (t2 > 1e-6) tau = std::min(tau, t2);
            if (tau < 1e50) { sol.tgo = tau; sol.ok = true; }
        }
    }

    if (!sol.ok) {
        // 기하학적 근사 fallback: t_go ≈ R / Vc(LOS)
        const double Rmag = norm3(R);
        const Vec3 rhat = (Rmag < 1e-12) ? Vec3{ 0,0,0 } : scale3(R, 1.0 / Rmag);
        const Vec3 v_rel_geom = sub3(t.v_t, scale3(m.u_m, m.V_m));
        const double Vc_geom = std::max(-dot3(v_rel_geom, rhat), 50.0); // 안전 하한
        sol.tgo = clip(Rmag / Vc_geom, 1e-3, 30.0);
        sol.ok = true; // 근사는 항상 유효로 간주
    }
    sol.pip = add3(t.r_t, scale3(t.v_t, sol.tgo));
    return sol;
}

Vec3
GuidanceFunction::runAutopilot(Vec3 a_cmd,
    const missile_state_t& m,
    const target_state_t& t,
    double dt)
{
    // per-thread 명령 프리필터 상태
    static thread_local Vec3 a_cmd_f{ 0,0,0 };

    // 파라미터 (MATLAB과 동일)
    constexpr double g0 = 9.81;
    constexpr double a_sat = 35.0 * g0;  // accel saturation
    constexpr double a_rate = 70.0 * g0;  // accel rate saturation
    constexpr double tau = 0.15;       // inner 1st-order
    constexpr double tau_cmd = 0.08;       // command prefilter
    constexpr double PI = 3.14159265358979323846;
    const     double omega_max = 45.0 * PI / 180.0; // rad/s

    // MIN_G 스케줄(레인지 기반)
    constexpr double MIN_G_far = 0.5 * g0;
    constexpr double MIN_G_near = 0.1 * g0;
    constexpr double R_MING_FAR = 1200.0;
    constexpr double R_MING_NEAR = 300.0;

    const Vec3 R = sub3(t.r_t, m.r_m);
    const double Rmag = norm3(R);
    const double phi_mg = (R_MING_FAR > R_MING_NEAR + 1e-9)
        ? clip((Rmag - R_MING_NEAR) / (R_MING_FAR - R_MING_NEAR), 0.0, 1.0)
        : 0.0;
    const double MIN_G = MIN_G_near + (MIN_G_far - MIN_G_near) * phi_mg;

    // (1) 횡가속만 사용
    a_cmd = sub3(a_cmd, scale3(m.u_m, dot3(a_cmd, m.u_m)));

    // (2) 명령 프리필터
    const double beta_cmd = dt / (tau_cmd + dt);
    a_cmd_f = add3(scale3(a_cmd_f, 1.0 - beta_cmd), scale3(a_cmd, beta_cmd));
    a_cmd = a_cmd_f;

    // (3) 회전율 프리리미트: |a|/V ≤ ω_max
    double w_cmd = norm3(a_cmd) / std::max(1.0, m.V_m);
    if (w_cmd > omega_max) a_cmd = scale3(a_cmd, omega_max / w_cmd);

    // (4) MIN_G 바닥 보장
    if (norm3(a_cmd) < MIN_G) {
        if (norm3(a_cmd) < 1e-9) {
            Vec3 ref = (std::abs(m.u_m[2]) < 0.9) ? Vec3{ 0,0,1 } : Vec3{ 0,1,0 };
            Vec3 n = cross3(m.u_m, ref);
            if (norm3(n) < 1e-9) n = cross3(m.u_m, Vec3{ 1,0,0 });
            a_cmd = scale3(unit3(n), MIN_G);
        }
        else {
            a_cmd = scale3(unit3(a_cmd), MIN_G);
        }
    }

    // (5) 내부 1차 응답 + rate/accel 포화
    Vec3 a_f = m.a_f_; // 이전 루프 응답가속
    const double alpha = dt / (tau + dt);
    Vec3 a_lpf = add3(a_f, scale3(sub3(a_cmd, a_f), alpha));
    Vec3 da = sub3(a_lpf, a_f);
    const double da_max = a_rate * dt;
    if (norm3(da) > da_max) da = scale3(da, da_max / norm3(da));
    Vec3 a_rl = add3(a_f, da);
    if (norm3(a_rl) > a_sat) a_rl = scale3(a_rl, a_sat / norm3(a_rl));

    return a_rl; // a_act
}
