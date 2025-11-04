//IGuidance.hpp 
#pragma once
#include "MissileStateManager.hpp"
#include "TargetStateManager.hpp"
#include "Utils.hpp"

class IGuidance {
public:
    //pure virtual
    virtual Vec3 calculateGuidance(missile_state_t, target_state_t, double) = 0;
};




