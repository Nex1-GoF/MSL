//TerminalGuidance.hpp 
#pragma once
#include "IGuidance.hpp"

class TerminalGuidance : public IGuidance {
public:
    Vec3 calculateGuidance(missile_state_t, target_state_t, double) override;

};