#define HANDOVER_INCLUDE_UTILITES

#include "handover.h"

char const *handover_tag_name(HandoverTag tag)
{
    switch (tag)
    {
#define TAG(NAME, VALUE)  \
    case HANDOVER_##NAME: \
        return #NAME;
        HANDOVER_TAGS(TAG)
#undef TAG
    }
    return "UNKNOWN";
}

bool handover_mergeable(uint32_t tag)
{
    switch (tag)
    {
    case HANDOVER_FREE:
    case HANDOVER_LOADER:
    case HANDOVER_KERNEL:
    case HANDOVER_RESERVED:
        return true;

    default:
        return false;
    }
}

bool handover_overlap(HandoverRecord lhs, HandoverRecord rhs)
{
    return lhs.start < rhs.start + rhs.size && rhs.start < lhs.start + lhs.size;
}

bool handover_just_before(HandoverRecord lhs, HandoverRecord rhs)
{
    return lhs.start + lhs.size == rhs.start;
}

bool handover_just_after(HandoverRecord lhs, HandoverRecord rhs)
{
    return lhs.start == rhs.start + rhs.size;
}

HandoverRecord handover_half_under(HandoverRecord self, HandoverRecord other)
{
    if (handover_overlap(self, other) &&
        self.start < other.start)
    {
        return (HandoverRecord){
            .tag = self.tag,
            .flags = 0,
            .start = self.start,
            .size = other.start - self.start,
        };
    }

    return (HandoverRecord){};
}

HandoverRecord handover_half_over(HandoverRecord self, HandoverRecord other)
{
    if (handover_overlap(self, other) &&
        self.start + self.size > other.start + other.size)
    {
        return (HandoverRecord){
            .tag = self.tag,
            .flags = 0,
            .start = other.start + other.size,
            .size = self.start + self.size - other.start - other.size,
        };
    }

    return (HandoverRecord){};
}

void handover_insert(HandoverPayload *payload, size_t index, HandoverRecord record)
{
    for (size_t i = payload->count; i > index; i--)
    {
        payload->records[i] = payload->records[i - 1];
    }

    payload->records[index] = record;
    payload->count++;
}

void handover_remove(HandoverPayload *payload, size_t index)
{
    for (size_t i = index; i < payload->count - 1; i++)
    {
        payload->records[i] = payload->records[i + 1];
    }

    payload->count--;
}

void handover_append(HandoverPayload *payload, HandoverRecord record)
{
    if (record.size == 0)
        return;

    for (size_t i = 0; i < payload->count; i++)
    {
        HandoverRecord *other = &payload->records[i];

        if (record.tag == other->tag &&
            handover_just_after(record, *other) &&
            handover_mergeable(record.tag))
        {
            other->size += record.size;
            return;
        }

        if (record.tag == other->tag &&
            handover_just_before(record, *other) &&
            handover_mergeable(record.tag))
        {
            other->start -= record.size;
            other->size += record.size;
            return;
        }

        if (handover_overlap(record, *other))
        {
            if (!handover_mergeable(record.tag))
            {
                HandoverRecord tmp = record;
                record = *other;
                *other = tmp;
            }

            HandoverRecord half_under = handover_half_under(record, *other);
            HandoverRecord half_over = handover_half_over(record, *other);

            handover_remove(payload, i);

            if (half_under.size)
                handover_insert(payload, i, half_under);

            if (half_over.size)
                handover_insert(payload, i, half_over);

            return;
        }

        if (record.start < other->start)
        {
            handover_insert(payload, i, record);
            return;
        }
    }

    payload->records[payload->count++] = record;
}

char const *handover_str(HandoverPayload const *payload, uint32_t offset)
{
    return (char const *)payload + offset;
}
