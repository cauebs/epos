// EPOS Raspberry Pi3 (Cortex-A53) Initialization

#include <system/config.h>
#include <machine.h>
#include <architecture/cpu.h>

__BEGIN_SYS

void Raspberry_Pi3::pre_init()
{
    static const unsigned int RESET             = Traits<Machine>::RESET; // must not be 0, otherwise, SEV won't wakeup secondary cores
    static const unsigned int MBOX_CTRL_BASE    = Memory_Map::MBOX_CTRL_BASE;
    static const unsigned int MBOX_CORE1_OFFSET = 0x9c;
    static const unsigned int MBOX_CORE2_OFFSET = 0xac;
    static const unsigned int MBOX_CORE3_OFFSET = 0xbc;

    // SMP initialization
    if(CPU::id() == 0) {
        // This is only a flat segment register that allows mapping the start point for the secondary cores
        // For the Send Event to take place, the start point is required to be non 0
        // Writing 0 to this register will trigger no effects at the target CPUs
        if(Traits<Build>::CPUS >= 2) {
            ASM("str %0, [%1, %2]" : : "p"(RESET), "p"(MBOX_CTRL_BASE), "i"(MBOX_CORE1_OFFSET) : );
            if(Traits<Build>::CPUS >= 3) {
                ASM("str %0, [%1, %2]" : : "p"(RESET), "p"(MBOX_CTRL_BASE), "i"(MBOX_CORE2_OFFSET) : );
                if(Traits<Build>::CPUS == 4) {
                    ASM("str %0, [%1, %2]" : : "p"(RESET), "p"(MBOX_CTRL_BASE), "i"(MBOX_CORE3_OFFSET) : );
                }
            }
        }

        // Wakeup secondary cores by sending an event (SEV)
        if(Traits<Build>::CPUS > 1)
            ASM ("SEV");
    }
}

__END_SYS
