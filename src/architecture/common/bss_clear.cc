extern unsigned int __bss_start;
extern unsigned int _end;

extern "C" void _bss_clear()
{
    unsigned int * ptr = &__bss_start;

    while(ptr < &_end)
        *ptr++ = 0;
}

