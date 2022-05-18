// EPOS RV32 PMU Mediator Initialization

#include <architecture/pmu.h>

#ifdef __sifive_u__

__BEGIN_SYS

void RV32_PMU::init()
{
    db<Init, PMU>(TRC) << "PMU::init()" << endl;
    
    for (auto channel = FIXED; channel < CHANNELS; ++channel) {
        reset(channel);
    }
}

__END_SYS

#endif
