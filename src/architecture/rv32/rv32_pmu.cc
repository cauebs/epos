// EPOS RV32 PMU Events Declaration

#include <architecture/pmu.h>

#ifdef __sifive_u__

__BEGIN_SYS

// Events of different classes cannot be set simultaneously to the same channel
const RV32_PMU::Event RV32_PMU::_events[RV32_PMU::EVENTS] = {
    // Class 0
    RV32_PMU::CLOCK_CYCLES,                                   // 0
    RV32_PMU::INSTRUCTIONS_RETIRED,                           // 1
    RV32_PMU::EXCEPTION_TAKEN,                                // 2
    RV32_PMU::INTEGER_LOAD_INSTRUCTION_RETIRED,               // 3
    RV32_PMU::INTEGER_STORE_INSTRUCTION_RETIRED,              // 4
    RV32_PMU::ATOMIC_MEMORY_OPERATION_RETIRED,                // 5
    RV32_PMU::SYSTEM_INSTRUCTION_RETIRED,                     // 6
    RV32_PMU::INTEGER_ARITHMETIC_INSTRUCTION_RETIRED,         // 7
    RV32_PMU::CONDITIONAL_BRANCH_RETIRED,                     // 8
    RV32_PMU::JAL_INSTRUCTION_RETIRED,                        // 9
    RV32_PMU::JALR_INSTRUCTION_RETIRED,                       // 10
    RV32_PMU::INTEGER_MULTIPLICATION_INSTRUCTION_RETIRED,     // 11
    RV32_PMU::INTEGER_DIVISION_INSTRUCTION_RETIRED,           // 12
    RV32_PMU::FLOATING_POINT_LOAD_INSTRUCTION_RETIRED,        // 13
    RV32_PMU::FLOATING_POINT_STORE_INSTRUCTION_RETIRED,       // 14
    RV32_PMU::FLOATING_POINT_ADDITION_RETIRED,                // 15
    RV32_PMU::FLOATING_POINT_MULTIPLICATION_RETIRED,          // 16
    RV32_PMU::FLOATING_POINT_FUSED_MULTIPLY_ADD_RETIRED,      // 17
    RV32_PMU::FLOATING_POINT_DIVISION_OR_SQUARE_ROOT_RETIRED, // 18
    RV32_PMU::OTHER_FLOATING_POINT_INSTRUCTION_RETIRED,       // 19

    // Class 1
    RV32_PMU::LOAD_USE_INTERLOCK,                             // 20
    RV32_PMU::LONG_LATENCY_INTERLOCK,                         // 21
    RV32_PMU::CSR_READ_INTERLOCK,                             // 22
    RV32_PMU::INSTRUCTION_CACHE_ITIM_BUSY,                    // 23
    RV32_PMU::DATA_CACHE_DTIM_BUSY,                           // 24
    RV32_PMU::BRANCH_DIRECTION_MISPREDICTION,                 // 25
    RV32_PMU::BRANCH_JUMP_TARGET_MISPREDICTION,               // 26
    RV32_PMU::PIPELINE_FLUSH_FROM_CSR_WRITE,                  // 27
    RV32_PMU::PIPELINE_FLUSH_FROM_OTHER_EVENT,                // 28
    RV32_PMU::INTEGER_MULTIPLICATION_INTERLOCK,               // 29
    RV32_PMU::FLOATING_POINT_INTERLOCK,                       // 30

    // Class 2
    RV32_PMU::INSTRUCTION_CACHE_MISS,                         // 31
    RV32_PMU::DATA_CACHE_MISS_OR_MEMORY_MAPPED_I_O_ACCESS,    // 32
    RV32_PMU::DATA_CACHE_WRITEBACK,                           // 33
    RV32_PMU::INSTRUCTION_TLB_MISS,                           // 34
    RV32_PMU::DATA_TLB_MISS,                                  // 35
};

__END_SYS

#endif
