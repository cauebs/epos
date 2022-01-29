// EPOS Raspberry Pi3 (ARM Cortex-A53) IC Mediator Implementation

#include <machine/machine.h>
#include <machine/ic.h>
#include <machine/timer.h>

__BEGIN_SYS

IC::Interrupt_Handler IC::_eoi_vector[INTS] = {
    0,
    Timer::eoi,         // System Timer C1
    0,
    0,                  // System Timer C3 (User_Timer)--> should we add the EOI function here?
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    Timer::eoi,         // ARM TIMER INT
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    // For the sake of the simplicity of the code, a single interrupt id is used to issue IPIs.
    // In this way, we only map a single handler, agnostic of MBOX number and Core ID.
    // On the other hand, handlers take Core ID into consideration when performing EOIs.
    // As a single MBOX handler is used to address all IPIs, we clear all MBOX of a Core in this handler.
    mailbox_eoi,        // MB0_CPU0
    0, // mailbox_eoi,  // MB1_CPU0
    0, // mailbox_eoi,  // MB2_CPU0
    0, // mailbox_eoi,  // MB3_CPU0
    0, // mailbox_eoi,  // MB0_CPU1
    0, // mailbox_eoi,  // MB1_CPU1
    0, // mailbox_eoi,  // MB2_CPU1
    0, // mailbox_eoi,  // MB3_CPU1
    0, // mailbox_eoi,  // MB0_CPU2
    0, // mailbox_eoi,  // MB1_CPU2
    0, // mailbox_eoi,  // MB2_CPU2
    0, // mailbox_eoi,  // MB3_CPU2
    0, // mailbox_eoi,  // MB0_CPU3
    0, // mailbox_eoi,  // MB1_CPU3
    0, // mailbox_eoi,  // MB2_CPU3
    0, // mailbox_eoi,  // MB3_CPU3
    Timer::eoi,         // CORE0 LOCAL TIMER INT = 117
    0
};

__END_SYS
