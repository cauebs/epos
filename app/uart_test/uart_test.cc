#include <utility/ostream.h>
#include __UART_H

using namespace EPOS;

OStream cout;

int main()
{
    auto uart = UART();

    auto message = "If you type, you should see the characters appear twice:\n";
    for (auto i = 0; message[i] != '\0'; ++i) {
        while (!uart.ready_to_put()) {}
        uart.put(message[i]);
    }
    uart.flush();

    while (true) {
        while (!uart.ready_to_get()) {}
        auto c = uart.get();
        uart.put(c);
        uart.put(c);
        uart.flush();
    }

    return 0;
}
