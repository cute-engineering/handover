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

# Machine State

# Records

## Free - `0x00000000`

## Self - `0xa24f988d`

## Stack - `0xf65b391b`

## Kernel - `0xbfc71b20`

## File - `0xcbc36d3b`

## RSDP - `0x8d3bbb`

## FDT - `0xb628bbc1`

## FB - `0xe2d55685`

## END - `0xffffffff`

# Definitions

- **C** - [handover.h](handover.h)
- **Rust** - [handover.rs](handover.rs)