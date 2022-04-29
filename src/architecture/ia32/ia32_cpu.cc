// EPOS IA32 CPU Mediator Implementation

#include <architecture/ia32/ia32_cpu.h>
#include <architecture/ia32/ia32_mmu.h>
#include <machine/ic.h>
#include <system/memory_map.h>

__BEGIN_SYS

volatile unsigned int CPU::_cores;
Hertz CPU::_cpu_clock;
Hertz CPU::_cpu_current_clock;
Hertz CPU::_bus_clock;

void CPU::Context::save() volatile
{
    // Save the running thread's context into its own stack (mostly for debugging)
    ASM("       push    %%ebp                                                           \n"
        "       mov     %%esp, %%ebp                                                    \n"
        "       mov     8(%%ebp), %%esp         # SP = this                             \n"
        "       add     $48, %%esp              # SP += sizeof(Context)                 \n"
        "       pushf                                                                   \n"
        "       push    %%cs                                                            \n"
        "       push    4(%%ebp)                # push IP                               \n"
        "       push    %%eax                                                           \n"
        "       push    %%ecx                                                           \n"
        "       push    %%edx                                                           \n"
        "       push    %%ebx                                                           \n"
        "       push    %%ebp                   # push ESP                              \n"
        "       push    (%%ebp)                 # push EBP                              \n"
        "       push    %%esi                                                           \n"
        "       push    %%edi                                                           \n"
        "       push    %0                      # push USP                              \n"
        "       mov     %%ebp, %%esp                                                    \n"
        "       pop     %%ebp                                                           \n"
        "       ret                                                                     \n" : "=m"(reinterpret_cast<TSS *>(Memory_Map::TSS0 + CPU::id() * sizeof(MMU::Page))->esp3) : );
}

void CPU::Context::load() const volatile
{
    // Reload Segment Registers with user-level selectors
if(Traits<System>::multitask)
    ASM("       mov     %0, %%ds                                                        \n"
        "       mov     %0, %%es                                                        \n"
        "       mov     %0, %%fs                                                        \n"
        "       mov     %0, %%gs                                                        \n" : : "r"(SEL_APP_DATA));

    // Pop the thread's context from the stack and update the user-level stack pointer in the dummy TSS
    sp(this);
    pop(true, false);
}

void CPU::switch_context(Context * volatile * o, Context * volatile n)
{
    // Context switches always happen inside the kernel, without crossing levels
    // So the context is organized to mimic the structure of a stack involved in a same-level exception handling (RPL=CPL),
    // that is, FLAGS, CS, and IP, so IRET will understand it


    // Recover the return address from the stack and save the previously running thread's context ("o") into its stack, including the user-level stack pointer automatically saved in the dummy TSS
    Context::push(true);
    ASM("       mov     48(%esp), %eax          # get address of parameter 'o'          \n"
        "       mov     %esp, (%eax  )          # update 'o' with the current SP        \n");

    // Restore the next thread's context ("n") from its stack and update the user-level stack pointer in the dummy TSS used for mode changes
    ASM("       mov     52(%esp), %esp          # get address of parameter 'n'         \n");
    Context::pop(true, false);
}

__END_SYS
