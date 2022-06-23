```
                   _    _                 _
                  | |  | |               | |
                  | |__| | __ _ _ __   __| | _____   _____ _ __
                  |  __  |/ _` | '_ \ / _` |/ _ \ \ / / _ \ '__|
                  | |  | | (_| | | | | (_| | (_) \ V /  __/ |
                  |_|  |_|\__,_|_| |_|\__,_|\___/ \_/ \___|_|

                     A simple and extensible boot protocol
```

# Design Goals

 - One contiguous area of memory makes it easy to hand it around.
 - Easy to parse using on contiguous array of records.
 - Centered around the memory map
 - No callbacks, no state.
# Overview

## Handover Request

```c
struct HandoverRequest
{
    uint32_t tag;
    uint32_t flags;
    uint64_t more;
};
```

## Handover Payload

```c
struct HandoverPayload
{
    uint32_t magic, agent, size, count;
    HandoverRecord records[];
};


struct HandoverRecord
{
    uint32_t tag;
    uint32_t flags;
    uint64_t start;
    uint64_t size;
    uint64_t more;
} ;
```

# Machine State

Except what stated bellow the machine state is undefined.

## x86_64

- rdi: will contain the 0xc001b001 magic number
- rsi: will contain the address of the handover payload in the kernel space

Paging is enabled and the following ranges are mapped
- Some LOADER regions are mapped to the same physical memory address for the initial spinup
- The first 4gio at 0xffffffff80000000 refered as I/O space
- KERNEL, STACK and SELF regions are mapped at 0xffff800000000000 and are refered as kernel space.

# Features

## FREE - `0x00000000`

Free for general use

## SELF - `0xa24f988d`

The handover structure

## STACK - `0xf65b391b`

CPU stack

## KERNEL - `0xbfc71b20`

Kernel region where the kernel code reside

## LOADER - `0xf1f80c26`

Memory allocated by the loaded and reclaimable by the kernel.

> This might include page-tables so make sure the create your own before freeing this

## FILE - `0xcbc36d3b`

Loaded file

## RSDP - `0x8d3bbb`

ACPI RSDP

## FDT - `0xb628bbc1`

Flatten device tree blob

## FB - `0xe2d55685`

Framebuffer

## END - `0xffffffff`

End marker

# Definitions

- **C** - [handover.h](handover.h)
- **Rust** - [handover.rs](handover.rs)
