//MidtermGuidance.hpp
#pragma once
#include "IGuidance.hpp"

class MidtermGuidance : public IGuidance {
public:
    Vec3 calculateGuidance(missile_state_t, target_state_t, double) override;

};

