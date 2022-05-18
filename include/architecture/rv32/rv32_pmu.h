// EPOS RV32 PMU Mediator Declarations

#ifndef __rv32_pmu_h
#define __rv32_pmu_h

#include <architecture/cpu.h>
#define __common_only__
#include <architecture/pmu.h>
#undef __common_only__

__BEGIN_SYS

class RV32_PMU: public PMU_Common
{
private:
    typedef CPU::Reg32 Reg32;
    typedef CPU::Reg64 Reg64;

protected:
    static const unsigned int CHANNELS = 4;
    static const unsigned int FIXED = 2;
    static const unsigned int EVENTS = 36;

public:
    // Fixed-function counters
    enum {
        CLOCK_CYCLES = 0,
        INSTRUCTIONS_RETIRED = 1,
    };

    // Predefined architectural performance events.
    enum {
        EXCEPTION_TAKEN                                = 0 | (1 << 8),
        INTEGER_LOAD_INSTRUCTION_RETIRED               = 0 | (1 << 9),
        INTEGER_STORE_INSTRUCTION_RETIRED              = 0 | (1 << 10),
        ATOMIC_MEMORY_OPERATION_RETIRED                = 0 | (1 << 11),
        SYSTEM_INSTRUCTION_RETIRED                     = 0 | (1 << 12),
        INTEGER_ARITHMETIC_INSTRUCTION_RETIRED         = 0 | (1 << 13),
        CONDITIONAL_BRANCH_RETIRED                     = 0 | (1 << 14),
        JAL_INSTRUCTION_RETIRED                        = 0 | (1 << 15),
        JALR_INSTRUCTION_RETIRED                       = 0 | (1 << 16),
        INTEGER_MULTIPLICATION_INSTRUCTION_RETIRED     = 0 | (1 << 17),
        INTEGER_DIVISION_INSTRUCTION_RETIRED           = 0 | (1 << 18),
        FLOATING_POINT_LOAD_INSTRUCTION_RETIRED        = 0 | (1 << 19),
        FLOATING_POINT_STORE_INSTRUCTION_RETIRED       = 0 | (1 << 20),
        FLOATING_POINT_ADDITION_RETIRED                = 0 | (1 << 21),
        FLOATING_POINT_MULTIPLICATION_RETIRED          = 0 | (1 << 22),
        FLOATING_POINT_FUSED_MULTIPLY_ADD_RETIRED      = 0 | (1 << 23),
        FLOATING_POINT_DIVISION_OR_SQUARE_ROOT_RETIRED = 0 | (1 << 24),
        OTHER_FLOATING_POINT_INSTRUCTION_RETIRED       = 0 | (1 << 25),

        LOAD_USE_INTERLOCK                             = 1 | (1 << 8),
        LONG_LATENCY_INTERLOCK                         = 1 | (1 << 9),
        CSR_READ_INTERLOCK                             = 1 | (1 << 10),
        INSTRUCTION_CACHE_ITIM_BUSY                    = 1 | (1 << 11),
        DATA_CACHE_DTIM_BUSY                           = 1 | (1 << 12),
        BRANCH_DIRECTION_MISPREDICTION                 = 1 | (1 << 13),
        BRANCH_JUMP_TARGET_MISPREDICTION               = 1 | (1 << 14),
        PIPELINE_FLUSH_FROM_CSR_WRITE                  = 1 | (1 << 15),
        PIPELINE_FLUSH_FROM_OTHER_EVENT                = 1 | (1 << 16),
        INTEGER_MULTIPLICATION_INTERLOCK               = 1 | (1 << 17),
        FLOATING_POINT_INTERLOCK                       = 1 | (1 << 18),

        INSTRUCTION_CACHE_MISS                         = 2 | (1 << 8),
        DATA_CACHE_MISS_OR_MEMORY_MAPPED_I_O_ACCESS    = 2 | (1 << 9),
        DATA_CACHE_WRITEBACK                           = 2 | (1 << 10),
        INSTRUCTION_TLB_MISS                           = 2 | (1 << 11),
        DATA_TLB_MISS                                  = 2 | (1 << 12),
    };

public:
    RV32_PMU() {}

    static void config(Channel channel, const Event event, Flags flags = NONE) {
        assert((static_cast<unsigned int>(channel) < CHANNELS) && (static_cast<unsigned int>(event) < EVENTS));
        db<PMU>(TRC) << "PMU::config(c=" << channel << ",e=" << event << ",f=" << flags << ")" << endl;

        if ((channel == CLOCK_CYCLES && event != CLOCK_CYCLES) || (channel == INSTRUCTIONS_RETIRED && event != INSTRUCTIONS_RETIRED)) {
            db<PMU>(WRN) << "PMU::config: channel " << channel << " is fixed in this architecture and cannot be reconfigured!" << endl;
            return;
        }

        csrs_mhpmevent(channel, _events[event]);
    }

    static Count read(Channel channel) {
        db<PMU>(TRC) << "PMU::read(c=" << channel << ")" << endl;

        if (channel == CLOCK_CYCLES) {
            return mcycle();
        }

        if (channel == INSTRUCTIONS_RETIRED) {
            return minstret();
        }

        return csrr_mhpmcounter(channel);
    }

    static void write(Channel channel, Count count) {
        db<PMU>(TRC) << "PMU::write(ch=" << channel << ",ct=" << count << ")" << endl;

        if (channel == CLOCK_CYCLES) {
            mcycle(count);
            return;
        }

        if (channel == INSTRUCTIONS_RETIRED) {
            minstret(count);
            return;
        }

        csrw_mhpmcounter(channel, count);
    }

    static void start(Channel channel) {
        db<PMU>(WRN) << "PMU::start: all channels start automatically in this architecture!" << endl;
    }

    static void stop(Channel channel) {
        if (channel < FIXED) {
            db<PMU>(WRN) << "PMU::stop: channel " << channel << " cannot be stopped in this architecture!" << endl;
            return;
        }

        db<PMU>(TRC) << "PMU::stop(c=" << channel << ")" << endl;
        csrw_mhpmevent(channel, 0);
    }

    static void reset(Channel channel) {
        db<PMU>(TRC) << "PMU::reset(c=" << channel << ")" << endl;
        write(channel, 0);
    }

    static void init();

private:
    static Reg64 csrr_mhpmcounter(Channel channel) {
        Reg32 high;
        Reg32 low;
        if (channel == 2) {
            ASM("csrr %0, mhpmcounter3h" : "=r"(high) : : );
            ASM("csrr %0, mhpmcounter3" : "=r"(low) : : );
        } else if (channel == 3) {
            ASM("csrr %0, mhpmcounter4h" : "=r"(high) : : );
            ASM("csrr %0, mhpmcounter4" : "=r"(low) : : );
        }
        return (Reg64(high) << 32) | Reg64(low);
    }

    static void csrw_mhpmcounter(Channel channel, Reg64 reg) {
        Reg32 high = reg >> 32;
        Reg32 low = reg;
        if (channel == 2) {
            ASM("csrw mhpmcounter3h, %0" : : "r"(high) : );
            ASM("csrw mhpmcounter3, %0" : : "r"(low) : );
        } else if (channel == 3) {
            ASM("csrw mhpmcounter4h, %0" : : "r"(high) : );
            ASM("csrw mhpmcounter4, %0" : : "r"(low) : );
        }
    }

    static void csrw_mhpmevent(Channel channel, Reg64 reg) {
        Reg32 high = reg >> 32;
        Reg32 low = reg;
        if (channel == 2) {
            ASM("csrw mhpmevent3h, %0" : : "r"(high) : );
            ASM("csrw mhpmevent3, %0" : : "r"(low) : );
        } else if (channel == 3) {
            ASM("csrw mhpmevent4h, %0" : : "r"(high) : );
            ASM("csrw mhpmevent4, %0" : : "r"(low) : );
        }
    }

    static void csrs_mhpmevent(Channel channel, Reg32 reg) {
        if (channel == 2) {
            ASM("csrs mhpmevent3, %0" : : "r"(reg) : );
        } else if (channel == 3) {
            ASM("csrs mhpmevent4, %0" : : "r"(reg) : );
        }
    }

    static Reg64 mcycle() {
    	Reg32 high;
        Reg32 low;
        ASM("rdcycleh %0" : "=r"(high) : : );
        ASM("rdcycle %0" : "=r"(low) : : );
        return (Reg64(high) << 32) | Reg64(low);
    }

    static void mcycle(Reg64 reg) {
    	Reg32 high = reg >> 32;
        Reg32 low = reg;
        ASM("csrw mcycleh, %0" : : "r"(high) : );
        ASM("csrw mcycle, %0" : : "r"(low) : );
    }

    static Reg64 minstret() {
    	Reg32 high;
        Reg32 low;
        ASM("rdinstreth %0" : "=r"(high) : : );
        ASM("rdinstret %0" : "=r"(low) : : );
        return (Reg64(high) << 32) | Reg64(low);
    }

    static void minstret(Reg64 reg) {
    	Reg32 high = reg >> 32;
        Reg32 low = reg;
        ASM("csrw minstreth, %0" : : "r"(high) : );
        ASM("csrw minstret, %0" : : "r"(low) : );
    }

private:
    static const Event _events[EVENTS];
};


class PMU: private RV32_PMU
{
    friend class CPU;

private:
    typedef RV32_PMU Engine;

public:
    using Engine::CHANNELS;
    using Engine::FIXED;
    using Engine::EVENTS;

    using Engine::Event;
    using Engine::Count;
    using Engine::Channel;

public:
    PMU() {}

    using Engine::config;
    using Engine::read;
    using Engine::write;
    using Engine::start;
    using Engine::stop;
    using Engine::reset;

private:
    static void init() { Engine::init(); }
};

__END_SYS

#endif
