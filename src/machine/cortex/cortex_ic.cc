// EPOS ARM Cortex-A IC Mediator Implementation

#include <machine/machine.h>
#include <machine/ic.h>
#include <machine/timer.h>
#include <process.h>

#ifdef __cortex_a__

extern "C" { void _int_entry() __attribute__ ((naked, nothrow, alias("_ZN4EPOS1S2IC5entryEv"))); }
extern "C" { void _eoi(unsigned int) __attribute__ ((alias("_ZN4EPOS1S2IC3eoiEj"))); }
extern "C" { void _reserved() __attribute__ ((alias("_ZN4EPOS1S2IC8reservedEv"))); }
extern "C" { void _undefined_instruction() __attribute__ ((alias("_ZN4EPOS1S2IC21undefined_instructionEv"))); }
extern "C" { void _software_interrupt() __attribute__ ((alias("_ZN4EPOS1S2IC18software_interruptEv"))); }
extern "C" { void _prefetch_abort() __attribute__ ((alias("_ZN4EPOS1S2IC14prefetch_abortEv"))); }
extern "C" { void _data_abort() __attribute__ ((alias("_ZN4EPOS1S2IC10data_abortEv"))); }
extern "C" { void _fiq() __attribute__ ((alias("_ZN4EPOS1S2IC3fiqEv"))); }
extern "C" { void __exit(); }

__BEGIN_SYS

IC::Interrupt_Handler IC::_int_vector[IC::INTS];

void IC::dispatch(unsigned int i)
{
    if((i != INT_SYS_TIMER) || Traits<IC>::hysterically_debugged)
        db<IC>(TRC) << "IC::dispatch(i=" << i << ")" << endl;

    assert(i < INTS);
    if(_eoi_vector[i])
        _eoi_vector[i](i);

    CPU::int_enable();  // Cortex-A disable interrupts at each interrupt

    _int_vector[i](i);
}

void IC::eoi(unsigned int id)
{
    if((id != INT_SYS_TIMER) || Traits<IC>::hysterically_debugged)
        db<IC>(TRC) << "IC::eoi(i=" << id << ")" << endl;

    assert(id < INTS);
    if(_eoi_vector[id])
        _eoi_vector[id](id);
}

void IC::int_not(Interrupt_Id i)
{
    db<IC>(WRN) << "IC::int_not(i=" << i << ")" << endl;
}

void IC::entry()
{
    // We assume A[T]PCS ARM ABI, so any function using registers r4 until r11 will save those upon beginning and restore them when exiting.
    // An interrupt can happen in the middle of one such function, but if the ISR drives the PC through other functions that use the same registers, they will save and restore them. We therefore don't need to save r4-r11 here.
    CPU::svc_enter(CPU::MODE_IRQ);
    dispatch(int_id());
    CPU::svc_leave();
}

void IC::prefetch_abort()
{
    // A prefetch abort on __exit is triggered by MAIN after returning to CRT0 and by the other threads after returning using the LR initialized at creation time to invoke Thread::exit()

    CPU::svc_enter(CPU::MODE_ABORT, false); // enter SVC to capture LR (the faulting address) in r1
    if((CPU::r1()) != CPU::Reg(&__exit))
        db<IC, Machine>(TRC) << "IC::prefetch_abort() [addr=" << CPU::r1() << "]" << endl;
    CPU::svc_stay();  // undo the context saving of svc_enter(), but do not leave SVC
    kill();
}

void IC::undefined_instruction()
{
    CPU::svc_enter(CPU::MODE_UNDEFINED, false); // enter SVC to capture LR (the faulting address) in r1
    db<IC>(TRC) << "IC::undefined_instruction(addr=" << CPU::r1() << ")" << endl;
    CPU::svc_stay();  // undo the context saving of svc_enter(), but do not leave SVC
    kill();
}

void IC::software_interrupt()
{
    CPU::svc_enter(CPU::MODE_UNDEFINED, false); // enter SVC to capture LR (the faulting address) in r1
    db<IC>(TRC) << "IC::software_interrupt(addr=" << CPU::r1() << ")" << endl;
    CPU::svc_stay();  // undo the context saving of svc_enter(), but do not leave SVC
    kill();
}

void IC::data_abort()
{
    CPU::svc_enter(CPU::MODE_ABORT, false); // enter SVC to capture LR (the faulting address) in r1
    db<IC, Machine>(TRC) << "IC::data_abort(addr=" << CPU::r1() << ")" << endl;
    CPU::svc_stay();  // undo the context saving of svc_enter(), but do not leave SVC
    kill();
}

void IC::reserved()
{
    CPU::svc_enter(CPU::MODE_ABORT, false); // enter SVC to capture LR (the faulting address) in r1
    db<IC, Machine>(WRN) << "IC::reserved/reset(addr=" << CPU::r1() << ")" << endl;
    CPU::svc_stay();  // undo the context saving of svc_enter(), but do not leave SVC
    Machine::panic();
}

void IC::fiq()
{
    CPU::svc_enter(CPU::MODE_FIQ, false); // enter SVC to capture LR (the return address) in r1
    db<IC>(ERR) << "IC::FIQ()" << endl;
    CPU::svc_stay();  // undo the context saving of svc_enter(), but do not leave SVC
    kill();
}

void IC::kill()
{
    db<IC>(WRN) << "The running thread will now be terminated!" << endl;
    Thread::exit(-1);
}

__END_SYS

#endif
