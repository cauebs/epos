// EPOS RISC V Initialization

#include <machine.h>

__BEGIN_SYS

void Machine::pre_init(System_Info * si)
{
    // Adjust stvec to point to _int_entry's logical address
    if(Traits<System>::multitask)
        CLINT::stvec(CLINT::DIRECT, &IC::entry);

    // Wake up other CPUs
    // Note that moving this code to SETUP would make RISC-V more similar to PC, but IC::_int_vector is in the BSS, so it's hard to prevent CPU 0 from zeroing it before others have finished booting
    if(Traits<System>::multicore && (CPU::id() == 0))
        smp_barrier_init(Traits<Build>::CPUS);

    if(CPU::id() == 0)
        Display::init();

    db<Init, Machine>(TRC) << "Machine::pre_init()" << endl;
}


void Machine::init()
{
    db<Init, Machine>(TRC) << "Machine::init()" << endl;

    if(Traits<IC>::enabled)
        IC::init();

    if(Traits<Timer>::enabled)
        Timer::init();
}

__END_SYS
