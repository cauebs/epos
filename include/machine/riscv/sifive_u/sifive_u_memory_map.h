// EPOS SiFive-U (RISC-V) Memory Map

#ifndef __riscv_sifive_u_memory_map_h
#define __riscv_sifive_u_memory_map_h


#include <system/memory_map.h>

__BEGIN_SYS

struct Memory_Map
{
private:
    static const bool multitask = Traits<System>::multitask;

public:
    enum : unsigned long {
        NOT_USED        = Traits<Machine>::NOT_USED,

        // Physical Memory
        RAM_BASE        = Traits<Machine>::RAM_BASE,
        RAM_TOP         = Traits<Machine>::RAM_TOP,
        MIO_BASE        = Traits<Machine>::MIO_BASE,
        MIO_TOP         = Traits<Machine>::MIO_TOP,
        INT_M2S         = RAM_TOP + 1 - 4096,   // the last page is used by the _int_m2s() interrupt forwarder installed by SETUP
        BOOT_STACK      = INT_M2S - Traits<Build>::CPUS * Traits<Machine>::STACK_SIZE, // will be used as the stack's base, not the stack pointer
        FREE_BASE       = RAM_BASE,
        FREE_TOP        = BOOT_STACK,

        // Memory-mapped devices
        TEST_BASE       = 0x00100000, // SiFive test engine
        RTC_BASE        = 0x00101000, // Goldfish RTC
        UART0_BASE      = 0x10000000, // NS16550A UART
        CLINT_BASE      = 0x02000000, // SiFive CLINT
        TIMER_BASE      = 0x02004000, // CLINT Timer
        PLIIC_CPU_BASE  = 0x0c000000, // SiFive PLIC

        // Physical Memory at Boot
        BOOT            = Traits<Machine>::BOOT,
        IMAGE           = Traits<Machine>::IMAGE,
        SETUP           = Traits<Machine>::SETUP,

        // Logical Address Space
        APP_LOW         = Traits<Machine>::APP_LOW,
        APP_HIGH        = Traits<Machine>::APP_HIGH,
        APP_CODE        = Traits<Machine>::APP_CODE,
        APP_DATA        = Traits<Machine>::APP_DATA,

        INIT            = Traits<Machine>::INIT,

        PHY_MEM         = Traits<Machine>::PHY_MEM,

        IO              = Traits<Machine>::IO,

        SYS             = Traits<Machine>::SYS,
        SYS_CODE        = multitask ? SYS + 0x00000000 : NOT_USED,
        SYS_INFO        = multitask ? SYS + 0x00100000 : NOT_USED,
        SYS_PT          = multitask ? SYS + 0x00101000 : NOT_USED,
        SYS_PD          = multitask ? SYS + 0x00102000 : NOT_USED,
        SYS_DATA        = multitask ? SYS + 0x00103000 : NOT_USED,
        SYS_STACK       = multitask ? SYS + 0x00200000 : NOT_USED,
        SYS_HEAP        = multitask ? SYS + 0x00400000 : NOT_USED,
        SYS_HIGH        = multitask ? SYS + 0x007fffff : NOT_USED
    };
};

__END_SYS

#endif
