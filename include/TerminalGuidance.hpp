//TerminalGuidance.hpp 
#pragma once
#include "IGuidance.hpp"
#include "GuidanceFunction.hpp"

class TerminalGuidance final : public IGuidance, protected GuidanceFunction {
public:
    Vec3 calculateGuidance(missile_state_t, target_state_t, double) override;

};