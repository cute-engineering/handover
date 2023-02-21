#ifndef HANDOVER_BUILDER_H_INCLUDED
#define HANDOVER_BUILDER_H_INCLUDED

#include <string.h>
#include "handover.h"

typedef struct
{
    void *buf;
    size_t size;
    char *string;
    HandoverPayload *payload;
} HandoverBuilder;

static inline void handover_builder_init(HandoverBuilder *builder, void *buf, size_t size)
{
    HandoverPayload *payload = (HandoverPayload *)buf;
    payload->size = size;

    *builder = (HandoverBuilder){
        .buf = buf,
        .size = size,
        .string = ((char *)buf) + size,
        .payload = payload,
    };
}

static inline void handover_builder_append(HandoverBuilder *builder, HandoverRecord record)
{
    builder->payload->records[builder->payload->count++] = record;
}

static inline size_t handover_builder_append_str(HandoverBuilder *builder, const char *str)
{
    size_t len = strlen(str) + 1;
    builder->string -= len;
    memcpy(builder->string, str, len);
    return builder->string - (char *)builder->buf;
}

#endif
