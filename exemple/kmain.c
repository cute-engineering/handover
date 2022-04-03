#define HANDOVER_INCLUDE_MACROS
#define HANDOVER_INCLUDE_UTILITIES

#include "../handover.h"

void kprintf(const char *format, ...);

HANDOVER(WITH_FB, WITH_FILES);

void _kmain(uint64_t magic, Handover *ho, int cpu)
{
    if (!handover_valid(ho))
    {
        kprintf("Handover is invalid!\n");
        return;
    }

    char const *ho_agent = handover_string(ho, ho->agent);

    if (!ho_agent)
    {
        kprintf("No boot agent specified\n");
        return;
    }

    kprintf("Bootagent: %s\n", ho_agent);
}
