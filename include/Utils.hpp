#pragma once
#include <array>
#include <cmath>
#include <chrono>

using namespace std::chrono_literals;
using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;
using Vec3 = std::array<double, 3>;
using Duration  = std::chrono::duration<double>;

// --- 계산 관련 벡터 유틸 ---
inline double dot3(const Vec3& a, const Vec3& b) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}
inline Vec3 cross3(const Vec3& a, const Vec3& b) {
    return { a[1]*b[2]-a[2]*b[1],
             a[2]*b[0]-a[0]*b[2],
             a[0]*b[1]-a[1]*b[0] };
}
inline double norm3(const Vec3& a) { return std::sqrt(dot3(a,a)); }
inline Vec3  add3 (const Vec3& a, const Vec3& b){ return {a[0]+b[0], a[1]+b[1], a[2]+b[2]}; }
inline Vec3  sub3 (const Vec3& a, const Vec3& b){ return {a[0]-b[0], a[1]-b[1], a[2]-b[2]}; }
inline Vec3  scale3(const Vec3& a, double s)    { return {a[0]*s, a[1]*s, a[2]*s}; }
inline Vec3  unit3 (const Vec3& v) {
    double n = norm3(v);
    return (n < 1e-12) ? Vec3{0,0,0} : Vec3{v[0]/n, v[1]/n, v[2]/n};
}
inline double clamp(double x, double a, double b){
    return x < a ? a : (x > b ? b : x);


}

static inline double clip(double x, double a, double b) { return x < a ? a : (x > b ? b : x); }

static inline double clip01(double x) { return clip(x, 0.0, 1.0); }



/* 형변환 관련 */

// 공통 스케일(1e7)
inline constexpr double kScale   = 1e7;
inline constexpr double kInvScale = 1.0 / kScale;

// ---- int32_t <-> double (간단 버전) ----
inline int32_t doubleToI32(double v) {
    return static_cast<int32_t>(std::llround(v * kScale));
}
inline double i32ToDouble(int32_t q) {
    return static_cast<double>(q) * kInvScale;
}

// ---- int16_t <-> double (간단 버전) ----
// *주의: int16_t는 범위가 좁습니다. (요청대로 포화/검사 없음)
inline int16_t doubleToI16(double v) {
    return static_cast<int16_t>(std::llround(v * kScale));
}
inline double i16ToDouble(int16_t q) {
    return static_cast<double>(q) * kInvScale;
}

