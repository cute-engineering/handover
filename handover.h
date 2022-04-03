
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Definitions ---------------------------------------------------------- */

#define HANDOVER_MAGIC (0xc001b001)

#define HANDOVER_FREE (0)

#define HANDOVER_SELF (0xa24f988d)

#define HANDOVER_STACK (0xf65b391b)

#define HANDOVER_KERNEL (0xbfc71b20)

#define HANDOVER_FILE (0xcbc36d3b)

#define HANDOVER_RSDP (0x8d3bbb)

#define HANDOVER_FDT (0xb628bbc1)

#define HANDOVER_FB (0xe2d55685)
#define HANDOVER_FB_RGBX32 (0x7451)
#define HANDOVER_FB_BGRX32 (0xd040)

#define HANDOVER_END (0xffffffff)

typedef struct
{
    uint32_t type;
    uint32_t flags;
    uint64_t start;
    uint64_t size;

    union
    {
        struct
        {
            uint16_t width;
            uint16_t height;
            uint16_t pitch;

            uint16_t format;
        } fb;

        struct
        {
            uint32_t low;
            uint32_t high;
        };

        uint64_t more;
    };
} HandoverRecord;

typedef struct
{
    uint32_t magic, agent, len, count;
    HandoverRecord records[];
} Handover;

typedef void HandoverEntry(uint64_t magic, Handover const *handover,
                           uint32_t cpu);

#ifdef HANDOVER_INCLUDE_UTILITIES

/* --- Utilities ------------------------------------------------------------ */

static inline bool handover_is_ordered(Handover const *handover)
{
    for (size_t i = 0; i < handover->count - 1; i++)
    {
        if (handover->records[i].start > handover->records[i + 1].start)
        {
            return false;
        }
    }
    return true;
}

static inline bool handover_valid(Handover const *handover)
{
    return (handover->magic == HANDOVER_MAGIC) &&
           (handover->records[handover->count].type == HANDOVER_END) &&
           handover_is_ordered(handover);
}

static inline char const *handover_string(Handover const *self,
                                          uint64_t offset)
{
    return (char const *)(self + 1) + sizeof(HandoverRecord) * self->count +
           offset;
}

static inline HandoverRecord *handover_record(Handover const *self,
                                              uint32_t index)
{
    return (HandoverRecord *)(self + 1) + index;
}

static inline HandoverRecord *handover_record_by_type(Handover const *self,
                                                      uint32_t type)
{
    for (uint32_t i = 0; i < self->count; ++i)
    {
        HandoverRecord *record = handover_record(self, i);
        if (record->type == type)
            return record;
    }

    return NULL;
}

#endif

/* --- Header --------------------------------------------------------------- */

#define HANDOVER_SECTION ".handover.header"

typedef struct
{
    uint32_t type;
    uint32_t flags;
    uint64_t more;
} HandoverRequest;

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
