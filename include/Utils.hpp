#pragma once
#include <array>
#include <cmath>
#include <chrono>
#include <algorithm>

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







/*
 * double <-> 정수형 변환 (고정 소수점 방식)
 *
 * - 32비트(int32, uint32): 1e7 (천만) 스케일 사용 (소수점 7자리)
 * - 16비트(int16): 1e4 (만) 스케일 사용 (소수점 4자리)
 * -> 16비트는 범위가 매우 좁으므로, 정밀도를 낮추고 범위를 확보합니다.
 */

// ---- 32비트용 스케일 (1e7) ----
inline constexpr double kScale32    = 1e7;
inline constexpr double kInvScale32 = 1.0 / kScale32;

// ---- 16비트용 스케일 (1e4) ----
inline constexpr double kScale16    = 1e4;
inline constexpr double kInvScale16 = 1.0 / kScale16;


// ======== 1. int32_t (부호 있는 32비트) ========

/**
 * @brief double을 int32_t로 변환 (1e7 스케일).
 * @warning 입력 double이 약 [-214.7, +214.7] 범위를 벗어나면 오버플로우 발생.
 */
inline int32_t doubleToI32(double v) {
    // 경고: 이 함수는 성능을 위해 범위 검사/포화를 수행하지 않습니다.
    // 범위 초과 시 값이 완전히 깨집니다.
    return static_cast<int32_t>(std::llround(v * kScale32));
}

inline double i32ToDouble(int32_t q) {
    return static_cast<double>(q) * kInvScale32;
}


// ======== 2. uint32_t (부호 없는 32비트) ========

/**
 * @brief double을 uint32_t로 변환 (1e7 스케일).
 * 음수는 0으로, 최대값 초과는 UINT32_MAX로 포화(saturate)시킵니다.
 */
inline uint32_t doubleToU32(double v) {
    if (v <= 0.0) {
        return 0;
    }
    
    // C++11 호환 포화 로직
    double scaled_v = v * kScale32;
    
    // <limits> 헤더의 std::numeric_limits를 사용하는 것이 더 C++ 스럽습니다.
    // #include <limits>
    // constexpr double max_val = static_cast<double>(std::numeric_limits<uint32_t>::max());
    
    // <cstdint> 헤더의 UINT32_MAX (C 스타일 매크로) 사용
    constexpr double max_val = static_cast<double>(UINT32_MAX);
    
    if (scaled_v >= max_val) {
        return UINT32_MAX;
    }
    
    return static_cast<uint32_t>(std::llround(scaled_v));
}

inline double u32ToDouble(uint32_t q) {
    return static_cast<double>(q) * kInvScale32;
}


// ======== 3. int16_t (부호 있는 16비트) ========

/**
 * @brief double을 int16_t로 변환 (1e4 스케일).
 * 범위를 벗어나는 값은 INT16_MIN/INT16_MAX로 포화(saturate)시킵니다.
 * @note 1e4 스케일 사용 시 유효 범위는 약 [-3.27, +3.27] 입니다.
 */
inline int16_t doubleToI16(double v) {
    double scaled_v = v * kScale16;

    // C++11 호환 포화 로직
    // <cstdint> 헤더의 INT16_MIN, INT16_MAX (C 스타일 매크로) 사용
    constexpr double min_val = static_cast<double>(INT16_MIN);
    constexpr double max_val = static_cast<double>(INT16_MAX);
    
    if (scaled_v <= min_val) {
        return INT16_MIN;
    }
    if (scaled_v >= max_val) {
        return INT16_MAX;
    }
    
    return static_cast<int16_t>(std::llround(scaled_v));
}

inline double i16ToDouble(int16_t q) {
    return static_cast<double>(q) * kInvScale16;
}