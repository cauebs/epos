// EPOS RISC-V UART Mediator Declarations

#ifndef __riscv_uart_h
#define __riscv_uart_h

#include <architecture/cpu.h>
#include <machine/uart.h>
#include <system/memory_map.h>

__BEGIN_SYS

class UART: private UART_Common
{
private:

    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg32 Reg32;

    static const unsigned int UNIT = Traits<UART>::DEF_UNIT;
    static const unsigned int BAUD_RATE = Traits<UART>::DEF_BAUD_RATE;
    static const unsigned int DATA_BITS = Traits<UART>::DEF_DATA_BITS;
    static const unsigned int PARITY = Traits<UART>::DEF_PARITY;
    static const unsigned int STOP_BITS = Traits<UART>::DEF_STOP_BITS;

    Reg32 rxdata_buffer;

    // UART registers offsets from UART_BASE
    enum {
        TXDATA          = 0x00,
        RXDATA          = 0x04,
        TXCTRL          = 0x08,
        RXCTRL          = 0x0C,
        IE              = 0x10,
        IP              = 0x14,
        DIV             = 0x18
    };

    // Useful bits from multiple registers
    enum {
        TXDATA_FULL            = 1 << 31,
        RXDATA_EMPTY           = 1 << 31,
        TXCTRL_ENABLE          = 1 << 0,
        TXCTRL_NSTOP           = 1 << 1,
        TXCTRL_TXCNT_WATERMARK = 7 << 16,
        RXCTRL_ENABLE          = 1 << 0,
        RXCTRL_TXCNT_WATERMARK = 7 << 16,
        IE_TXWM                = 1 << 0,
        IE_RXWM                = 1 << 1,
        IP_TXWM                = 1 << 0,
        IP_RXWM                = 1 << 1
    };

public:
    using UART_Common::NONE;
    using UART_Common::EVEN;
    using UART_Common::ODD;

public:
    UART(unsigned int unit = UNIT, unsigned int baud_rate = BAUD_RATE, unsigned int data_bits = DATA_BITS, unsigned int parity = PARITY, unsigned int stop_bits = STOP_BITS) {
        config(baud_rate, data_bits, parity, stop_bits);
    }

    void config(unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits) {
        
        reg(TXCTRL) = TXCTRL_ENABLE | stop_bits;
        reg(RXCTRL) = RXCTRL_TXCNT_WATERMARK | RXCTRL_ENABLE;

        Reg32 br = Traits<UART>::CLOCK / BAUD_RATE;
        if (Traits<UART>::CLOCK / br > BAUD_RATE) {
            br += 1;
        } // get ceiling
        reg(DIV) = br & 0xffff;

        rxdata_buffer = 0 | RXDATA_EMPTY;

    }

    void config(unsigned int * baud_rate, unsigned int * data_bits, unsigned int * parity, unsigned int * stop_bits) {
        unsigned int div = Reg32(reg(DIV));
        *baud_rate = Traits<UART>::CLOCK / div;
        *stop_bits = (Reg32(reg(TXCTRL) & TXCTRL_NSTOP) >> 1) + 1;
    }

    bool rxd_empty() { 
        rxdata_buffer = reg(RXDATA);
        return (rxdata_buffer & RXDATA_EMPTY) ;
        
        if ((rxdata_buffer & RXDATA_EMPTY)) { 
            rxdata_buffer = reg(RXDATA);
            return (rxdata_buffer & RXDATA_EMPTY) ;
        } else {
            return false;        
        }
    }


    bool txd_full() {
        return (reg(TXDATA) & TXDATA_FULL);
    }

    char get() {
        while(rxd_empty());
        char ret = rxdata_buffer & 0xff;
        rxdata_buffer = 0 | RXDATA_EMPTY;
        return ret;
    }

    void put(char c) {
        while(txd_full());
        reg(TXDATA) = c;
    }

    void flush() { }
    bool ready_to_get() { return !rxd_empty(); }
    bool ready_to_put() { return true; } //!txd_full(); }

    void int_enable(bool receive = true, bool transmit = true, bool line = true, bool modem = true) {
        // reg(IER) = receive | (transmit << 1) | (line << 2) | (modem << 3);
    }
    void int_disable(bool receive = true, bool transmit = true, bool line = true, bool modem = true) {
        // reg(IER) = reg(IER) & ~(receive | (transmit << 1) | (line << 2) | (modem << 3));
    }

    void reset() {
        // Reconfiguring the UART implicitly resets it
        unsigned int b, db, p, sb;
        config(&b, &db, &p, &sb);
        config(b, db, p, sb);
    }

    void loopback(bool flag) {
        // Reg8 mask = 0xff;
        // mask -= LOOPBACK_MASK;
        // mask += (flag << 4); // if 1, restore flag, else make it disable loopback
        // reg(MCR) = reg(MCR) & mask; 
    }

    void power(const Power_Mode & mode) {}

private:
    static void init() {}

    static volatile CPU::Reg32 & reg(unsigned int o) { return reinterpret_cast<volatile CPU::Reg32 *>(Memory_Map::UART_BASE)[o / sizeof(CPU::Reg32)]; }
};

__END_SYS

#endif
