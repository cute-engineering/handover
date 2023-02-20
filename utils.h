#ifndef HANDOVER_UTILS_H_INCLUDED
#define HANDOVER_UTILS_H_INCLUDED

#include <stddef.h>
#include <string.h>
#include "handover.h"

static inline HandoverRecord handover_file_find(HandoverPayload *handover, char const *name)
{
    for (size_t i = 0; i < handover->count; i++)
    {
        HandoverRecord record = handover->records[i];

        if (record.tag == HANDOVER_FILE)
        {
            char *filename = (char *)handover + record.file.name;

            if (memcmp(filename, name, strlen(filename)) == 0)
            {
                return record;
            }
        }
    }

    return (HandoverRecord){};
}

#define handover_foreach_record(h, r)                        \
    if ((h)->count > 0)                                      \
        for (size_t i = 0;                                   \
             i < (h)->count && (((r) = (h)->records[i]), 1); \
             ++i)

#endif