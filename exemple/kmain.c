#define HANDOVER_INCLUDE_MACROS
#define HANDOVER_INCLUDE_UTILITES

#include <stdarg.h>
#include "../handover.h"

HANDOVER(WITH_FB, WITH_FILES);

// simple printf implementation that supports %s %d %x %p

size_t cstrlen(const char *str)
{
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

void putc(char c)
{
    asm volatile("outb %0, %1"
                 :
                 : "a"(c), "Nd"(0xe9));
}

void cstrrev(char s[])
{
    for (int i = 0, j = cstrlen(s) - 1; i < j; i++, j--)
    {
        char c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void itoa(int n, char s[], int base)
{
    int i, sign;

    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do
    {
        s[i++] = n % base + '0';
    } while ((n /= base) > 0);

    if (sign < 0)
        s[i++] = '-';

    s[i] = '\0';
    cstrrev(s);
}

void puts(const char *s)
{
    while (*s)
    {
        putc(*s);
        s++;
    }
}

void printf(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;

            if (*fmt == 's')
            {
                puts(va_arg(va, const char *));
            }
            else if (*fmt == 'd')
            {
                int number = va_arg(va, int);
                if (number < 0)
                {
                    putc('-');
                    number = -number;
                }
                char buffer[sizeof(int) * 8 + 1];
                itoa(number, buffer, 10);
                puts(buffer);
            }
            else if (*fmt == 'x')
            {
                int number = va_arg(va, int);
                char buffer[sizeof(size_t) * 8 + 1];
                itoa(number, buffer, 16);
                puts(buffer);
            }
            else if (*fmt == '%')
            {
                putc('%');
            }
        }
        else
        {
            putc(*fmt);
        }

        fmt++;
    }

    va_end(va);
}

void _kmain(uint64_t magic, HandoverPayload *payload)
{
    printf("Handover magic: %x", magic);
    printf("Handover payload: %x", payload);

    printf("Handover agent: %s", payload->agent);
    printf("Handover size: %d", payload->size);
    printf("Handover count: %d", payload->count);

    for (size_t i = 0; i < payload->count; i++)
    {
        HandoverRecord record = payload->records[i];
        printf("Handover tag: %s(%x)", handover_tag_name(record.tag), record.tag);
        printf("    flags: %x", record.flags);
        printf("    start: %d", record.start);
        printf("    size: %d", record.size);
        printf("    more: %x", record.more);
    }
}
