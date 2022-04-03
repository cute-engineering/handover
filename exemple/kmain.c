#define HANDOVER_INCLUDE_MACROS
#define HANDOVER_INCLUDE_UTILITIES
#define HANDOVER_INCLUDE_HEADER

#include "../handover.h"

void ho_printf(void *ctx, char const *format, ...);

HANDOVER(WITH_FB, WITH_FILES);

void _kmain(uint64_t magic, Handover *handover)
{
    handover_dump(handover, ho_printf, NULL);
}
