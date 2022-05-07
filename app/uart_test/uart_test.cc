#include <utility/ostream.h>
#include __UART_H

using namespace EPOS;

OStream cout;

int main()
{
    auto uart = UART();

    auto message = "Hello, world!\n";
    for (auto i = 0; message[i] != '\0'; ++i) {
        uart.put(message[i]);
    }
    uart.flush();

    return 0;
}
