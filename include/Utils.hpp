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
