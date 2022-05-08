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
    typedef CPU::Reg16 Reg16;
    typedef CPU::Reg32 Reg32;

    static const unsigned int UNIT = Traits<UART>::DEF_UNIT;
    static const unsigned int BAUD_RATE = Traits<UART>::DEF_BAUD_RATE;
    static const unsigned int DATA_BITS = Traits<UART>::DEF_DATA_BITS;
    static const unsigned int PARITY = Traits<UART>::DEF_PARITY;
    static const unsigned int STOP_BITS = Traits<UART>::DEF_STOP_BITS;

    // UART registers offsets from UART_BASE
    enum {
        TXD = 0x00,
        RXD = 0x04,
        TXCTRL = 0x08,
        RXCTRL = 0x0C,
        IE     = 0x10,
        IP     = 0x14,
        DIV    = 0x18,
    };

    // Useful bits from multiple registers
    enum {
        TXD_FULL = 31,
        RXD_EMPTY = 31,

        TXCTRL_TXEN = 0,
        TXCTRL_NSTOP = 1,
        TXCTRL_TXCNT = 16,

        RXCTRL_RXEN = 0,
        RXCTRL_RXCNT = 16,

        IE_TXWM = 0,
        IE_RXWM = 1,

        IP_TXWM = 0,
        IP_RXWM = 1,
    };

    Reg8 _rx_buffer;
    bool _rx_buffer_filled = false;

public:
    using UART_Common::NONE;
    using UART_Common::EVEN;
    using UART_Common::ODD;

public:
    UART(unsigned int unit = UNIT, unsigned int baud_rate = BAUD_RATE, unsigned int data_bits = DATA_BITS, unsigned int parity = PARITY, unsigned int stop_bits = STOP_BITS) {
        config(baud_rate, data_bits, parity, stop_bits);
    }

    void config(unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits) {
        auto div = Reg16(Traits<UART>::CLOCK / baud_rate - 1);
        reg(DIV) = div;

        assert(stop_bits == 1 || stop_bits == 2);
        if (stop_bits == 1) {
            reg(TXCTRL) &= ~(1 << TXCTRL_NSTOP);
        } else if (stop_bits == 2) {
            reg(TXCTRL) |= (1 << TXCTRL_NSTOP);
        }

        enable_tx_watermark(1); // used for flushing

        reg(TXCTRL) |= (1 << TXCTRL_TXEN);
        reg(RXCTRL) |= (1 << RXCTRL_RXEN);
    }

    void config(unsigned int * baud_rate, unsigned int * data_bits, unsigned int * parity, unsigned int * stop_bits) {
        unsigned int div = Reg16(reg(DIV));
        *baud_rate = Traits<UART>::CLOCK / (div + 1);

        if (reg(TXCTRL) & TXCTRL_NSTOP) {
            *stop_bits = 2;
        } else {
            *stop_bits = 1;
        }
    }

    Reg8 rxd() { 
        if (_rx_buffer_filled) {
            _rx_buffer_filled = false;
            return _rx_buffer;
        }
        return reg(RXD);
    }
    void txd(Reg8 c) { 
        reg(TXD) = c;
    }

    bool rxd_empty() {
        if (_rx_buffer_filled) {
            return false;
        }

        auto received = reg(RXD);
        if (received & (1 << RXD_EMPTY)) {
            return true;
        }

        _rx_buffer = Reg8(received);
        _rx_buffer_filled = true;
        return false;
    }

    bool txd_full() {
        return (reg(TXD) & (1 << TXD_FULL));
    }

    bool rxd_full() { return false; } //still not defined
    
    bool txd_empty() { 
        return (reg(IP) & (1 << IP_TXWM));
    }

    bool busy() {
        return false; // not applicable
    }

    char get() { while(!ready_to_get()); return rxd(); }
    void put(char c) { while(!ready_to_put()); txd(c); }

    void flush() { while(!txd_empty()); }
    bool ready_to_get() { return !rxd_empty(); }
    bool ready_to_put() { return !txd_full(); }

    void reset() {
        // Reconfiguring the UART implicitly resets it
        unsigned int b, db, p, sb;
        config(&b, &db, &p, &sb);
        config(b, db, p, sb);
    }

    void power(const Power_Mode & mode) {}

private:
    static void init() {}

    static volatile CPU::Reg32 & reg(unsigned int o) { return reinterpret_cast<volatile CPU::Reg32 *>(Memory_Map::UART_BASE)[o / sizeof(CPU::Reg32)]; }

    void enable_tx_watermark(unsigned int level) {
        reg(TXCTRL) |= (level << TXCTRL_TXCNT);
        reg(IE) |= (1 << IE_TXWM);
    }
};

__END_SYS

#endif
