#ifndef HANDOVER_H_INCLUDED
#define HANDOVER_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define HANDOVER_KERNEL_BASE 0xffffffff80000000
#define HANDOVER_UPPER_HALF 0xffff800000000000

#define HANDOVER_COOLBOOT 0xc001b001
#define HANDOVER_SECTION ".handover"

#define HANDOVER_TAGS(TAG)        \
    TAG(FREE, 0x00000000)         \
    TAG(MAGIC, HANDOVER_COOLBOOT) \
    TAG(SELF, 0xa24f988d)         \
    TAG(STACK, 0xf65b391b)        \
    TAG(KERNEL, 0xbfc71b20)       \
    TAG(LOADER, 0xf1f80c26)       \
    TAG(FILE, 0xcbc36d3b)         \
    TAG(RSDP, 0x8ef29c18)         \
    TAG(FDT, 0xb628bbc1)          \
    TAG(FB, 0xe2d55685)           \
    TAG(CMDLINE, 0x435140c4)      \
    TAG(RESERVED, 0xb8841d2d)     \
    TAG(END, 0xffffffff)

typedef enum
{
#define TAG(NAME, VALUE) HANDOVER_##NAME = VALUE,
    HANDOVER_TAGS(TAG)
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
        struct
        {
            uint16_t width;
            uint16_t height;
            uint16_t pitch;

#define HANDOVER_RGBX8888 0x7451
#define HANDOVER_BGRX8888 0xd040
            uint16_t format;
        } fb;

        struct
        {
            uint32_t name;
            uint32_t meta;
        } file;

        uint64_t more;
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

#    define HANDOVER_REQ_START    \
        {                         \
            .tag = HANDOVER_MAGIC \
        }

#    define HANDOVER_REQ_END    \
        {                       \
            .tag = HANDOVER_END \
        }

#    define WITH_FB            \
        {                      \
            .tag = HANDOVER_FB \
        }

#    define WITH_ACPI            \
        {                        \
            .tag = HANDOVER_RSDP \
        }

#    define WITH_FDT            \
        {                       \
            .tag = HANDOVER_FDT \
        }

#    define WITH_FILES           \
        {                        \
            .tag = HANDOVER_FILE \
        }

#    define WITH_CMDLINE            \
        {                           \
            .tag = HANDOVER_CMDLINE \
        }

#    define HANDOVER(...)                                                  \
        __attribute__((section(HANDOVER_SECTION),                          \
                       used)) static HandoverRequest handover_header[] = { \
            {.tag = HANDOVER_MAGIC},                                       \
            __VA_ARGS__ __VA_OPT__(, ){.tag = HANDOVER_END},               \
        };

#endif

/* --- Utilities ------------------------------------------------------------ */

#ifdef HANDOVER_INCLUDE_UTILITES

char const *handover_tag_name(uint32_t tag);

bool handover_mergeable(uint32_t tag);

bool handover_overlap(HandoverRecord lhs, HandoverRecord rhs);

bool handover_just_before(HandoverRecord lhs, HandoverRecord rhs);

bool handover_just_after(HandoverRecord lhs, HandoverRecord rhs);

HandoverRecord handover_half_under(HandoverRecord self, HandoverRecord other);

HandoverRecord handover_half_over(HandoverRecord self, HandoverRecord other);

void handover_insert(HandoverPayload *payload, size_t index, HandoverRecord record);

void handover_remove(HandoverPayload *payload, size_t index);

void handover_append(HandoverPayload *payload, HandoverRecord record);

char const *handover_str(HandoverPayload const *payload, uint32_t offset);

#endif

#endif
