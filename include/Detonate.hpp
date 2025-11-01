//Detonate.hpp
#include "IGuidance.hpp"

class Detonate : public IGuidance {
public:
    Vec3 calculateGuidance(missile_state_t, target_state_t, double) override;
    
};