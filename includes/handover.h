#ifndef HANDOVER_H_INCLUDED
#define HANDOVER_H_INCLUDED

#include <stdint.h>

#define HANDOVER_KERNEL_BASE = 0xffffffff80000000;
#define HANDOVER_UPPER_HALF = 0xffff800000000000;

#define HANDOVER_COOLBOOT 0xc001b001
#define HANDOVER_SECTION ".handover"

#define HANDOVER_FOREACH_TAGS(TAG) \
    TAG(FREE, 0x00000000)          \
    TAG(MAGIC, HANDOVER_COOLBOOT)  \
    TAG(SELF, 0xa24f988d)          \
    TAG(STACK, 0xf65b391b)         \
    TAG(KERNEL, 0xbfc71b20)        \
    TAG(LOADER, 0xf1f80c26)        \
    TAG(FILE, 0xcbc36d3b)          \
    TAG(RSDP, 0x8ef29c18)          \
    TAG(FDT, 0xb628bbc1)           \
    TAG(FB, 0xe2d55685)            \
    TAG(END, 0xffffffff)

typedef enum
{
#define TAG(NAME, VALUE) HANDOVER_##NAME = VALUE,
    HANDOVER_FOREACH_TAGS(TAG)
#undef TAG
} HandoverTag;

typedef struct
{
    uint32_t tag;
    uint32_t flags;
    uint64_t start;
    uint64_t size;
    union
    {
        uint64_t more;
        struct
        {
            uint32_t car;
            uint32_t cdr;
        };
    };
} HandoverRecord;

typedef struct
{
    uint32_t magic, agent, size, count;
    HandoverRecord records[];
} HandoverPayload;

typedef struct
{
    uint32_t tag;
    uint32_t flags;
    uint64_t more;
} HandoverRequest;

typedef void HandoverEntry(
    uint64_t magic,
    HandoverPayload const *payload);

/* --- Header Utilities ----------------------------------------------------- */

#ifdef HANDOVER_INCLUDE_MACROS
#    define HANDOVER_REQ_START     \
        {                          \
            .type = HANDOVER_MAGIC \
        }

#    define HANDOVER_REQ_END     \
        {                        \
            .type = HANDOVER_END \
        }

#    define WITH_FB             \
        {                       \
            .type = HANDOVER_FB \
        }

#    define WITH_ACPI             \
        {                         \
            .type = HANDOVER_RSDP \
        }

#    define WITH_FDT             \
        {                        \
            .type = HANDOVER_FDT \
        }

#    define WITH_FILES            \
        {                         \
            .type = HANDOVER_FILE \
        }

#    define HANDOVER(...)                                                        \
        __attribute__((section(HANDOVER_SECTION),                                \
                       used)) static HandoverRequest const handover_header[] = { \
            {.type = HANDOVER_MAGIC},                                            \
            __VA_ARGS__ __VA_OPT__(, ){.type = HANDOVER_END},                    \
        };

#endif

/* --- Utilities ------------------------------------------------------------ */

#ifdef HANDOVER_INCLUDE_UTILITES

static inline char const *handover_tag_name(HandoverTag tag)
{
    switch (tag)
    {
#    define TAG(NAME, VALUE) \
    case HANDOVER_##NAME:    \
        return #NAME;
        HANDOVER_FOREACH_TAGS(TAG)
#    undef TAG
    }
    return "UNKNOWN";
}

#endif

#endif
