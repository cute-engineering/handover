# Handover Protocol

## 1. Introduction

This document specifies the Handover Protocol, a mechanism designed to facilitate the transfer of control and essential system information from a bootloader or early-stage environment (the "boot agent") to a more fully-fledged kernel environment. The protocol centers around a structured memory region that contains metadata about crucial system resources, enabling the kernel to discover and initialize these resources during its startup phase.

## 2. Terminology

* **Boot Agent:** The entity responsible for populating the handover data structure and transferring control to the kernel.
* **Kernel:** The operating system kernel that receives control and system information from the boot agent.
* **Handover Payload:** A structured memory region containing metadata about system resources.
* **Handover Record:** An individual entry within the Handover Payload, describing a specific system resource.
* **Handover Tag:** An identifier associated with each Handover Record, specifying the type of resource it represents.

## 3. Handover Payload Structure

The Handover Payload is a contiguous memory region with the following structure:

```c
struct HandoverPayload {
    uint32_t magic;      // Magic value (0xc001b001)
    uint32_t agent;      // Offset to the boot agent name string within the string table
    uint32_t size;       // Total size of the payload in bytes
    uint32_t count;      // Number of Handover Records
    HandoverRecord records[]; // Array of Handover Records
    // String table immediately follows the records array
};
```

Each `HandoverRecord` has the following format:

```c
struct HandoverRecord {
    uint32_t tag;        // Handover Tag identifying the resource type
    uint32_t flags;      // Reserved for future use
    uint64_t start;      // Starting address of the resource in memory
    uint64_t size;       // Size of the resource in bytes
    union {
        struct {            // Framebuffer-specific information
            uint16_t width;
            uint16_t height;
            uint16_t pitch;
            uint16_t format;
        } fb;
        struct {            // File-specific information
            uint32_t name;  // Offset to filename string within the string table
            uint32_t meta;  // Offset to additional metadata within the string table
        } file;
        uint64_t more;     // Additional data, depending on the tag
    };
};
```

The string table is located immediately after the `records` array within the `HandoverPayload`. It stores null-terminated strings referenced by offsets within `HandoverRecord` structures (e.g., the `name` field in the `file` union member).

## 4. Handover Tags

The following Handover Tags are defined:

* **`HANDOVER_FREE` (0x00000000)**
    * Represents a free memory region available for general use by the kernel.
    * `start`: Starting address of the free region.
    * `size`: Size of the free region in bytes.
    * Other fields are unused and set to zero.

* **`HANDOVER_SELF` (0xa24f988d)**
    * Identifies the `HandoverPayload` structure itself.
    * `start`: Starting address of the `HandoverPayload`.
    * `size`: Total size of the `HandoverPayload` in bytes.
    * Other fields are unused and set to zero.

* **`HANDOVER_STACK` (0xf65b391b)**
    * Specifies the location and size of the initial kernel stack.
    * `start`: Starting address of the kernel stack.
    * `size`: Size of the kernel stack in bytes.
    * Other fields are unused and set to zero.

* **`HANDOVER_KERNEL` (0xbfc71b20)**
    * Marks the memory region where the kernel code resides.
    * `start`: Starting address of the kernel code region.
    * `size`: Size of the kernel code region in bytes.
    * Other fields are unused and set to zero.

* **`HANDOVER_LOADER` (0xf1f80c26)**
    * Identifies memory regions allocated by the bootloader, which may be reclaimable by the kernel.
    * `start`: Starting address of the allocated memory region.
    * `size`: Size of the allocated memory region in bytes.
    * Other fields are unused and set to zero.

* **`HANDOVER_FILE` (0xcbc36d3b)**
    * Represents a loaded file.
    * `start`: Starting address of the file in memory.
    * `size`: Size of the file in bytes.
    * `file.name`: Offset to the null-terminated filename string within the string table.
    * `file.meta`: Offset to additional metadata within the string table.

    > **Non-Normative Note:** It's good practice to encode file metadata in a structured format (e.g., JSON) to facilitate parsing and interpretation.

* **`HANDOVER_RSDP` (0x8ef29c18)**
    * Points to the ACPI RSDP (Root System Description Pointer) structure.
    * `start`: Physical address of the RSDP structure.
    * `size`: Size of the RSDP structure (typically 20 bytes).
    * Other fields are unused and set to zero.

* **`HANDOVER_FDT` (0xb628bbc1)**
    * Specifies the location of a Flattened Device Tree (FDT) blob.
    * `start`: Starting address of the FDT blob in memory.
    * `size`: Size of the FDT blob in bytes.
    * Other fields are unused and set to zero.

* **`HANDOVER_FB` (0xe2d55685)**
    * Provides information about an available framebuffer.
    * `start`: Starting address of the framebuffer in memory.
    * `size`: Size of the framebuffer in bytes.
    * `fb.width`: Width of the framebuffer in pixels.
    * `fb.height`: Height of the framebuffer in pixels.
* `fb.pitch`: Number of bytes per scanline in the framebuffer.
    * `fb.format`: Pixel format of the framebuffer (e.g., `HANDOVER_RGBX8888` or `HANDOVER_BGRX8888`).

* **`HANDOVER_CMDLINE` (0x435140c4)**
    * Stores the kernel command line arguments.
    * `meta`: Offset to the null-terminated command line string within the string table.
    * Other fields are unused and set to zero.
    > **Non-Normative Note:** It's good practice to encode command line arguments in a structured format (e.g., JSON) to facilitate parsing and interpretation.

* **`HANDOVER_RESERVED` (0xb8841d2d)**
    * Marks a reserved memory region.
    * `start`: Starting address of the reserved memory region.
    * `size`: Size of the reserved memory region in bytes.
    * Other fields are unused and set to zero.

* **`HANDOVER_END` (0xffffffff)**
    * Signals the end of the `HandoverPayload` structure.
    * Other fields are not applicable and set to zero.

## 5. Handover Process

1. The boot agent populates a `HandoverPayload` structure in memory, including information about available system resources. This includes placing null-terminated strings in the string table at the end of the payload and setting appropriate offsets in relevant `HandoverRecord` structures.
2. The boot agent ensures that the following machine state conditions are met:
    * Architecture: x86_64
    * Registers:
        * `rdi`: Contains the magic value `0xc001b001`
        * `rsi`: Points to the `HandoverPayload` in kernel space
    * Paging: Enabled
    * Memory Mapping:
        * I/O space: First 4 GiB at `0xffffffff80000000`
        * Kernel space: `KERNEL`, `STACK`, and `SELF` regions mapped at `0xffff800000000000`
        * Some `LOADER` regions may be mapped to the same physical addresses initially
3. The boot agent transfers control to the kernel's entry point.
4. The kernel parses the `HandoverPayload`.
5. When encountering a `HandoverRecord` with a tag like `HANDOVER_FILE` that references a string offset, the kernel accesses the corresponding null-terminated string from the string table located at the end of the `HandoverPayload`.

## 6. Security Considerations

The current version of the Handover Protocol does not include explicit security mechanisms. Implementations should consider incorporating measures like digital signatures or checksums to ensure the integrity and authenticity of the handover data, especially in environments where security is critical.

## 7. Future Extensions

* **Error Handling:** The protocol could be enhanced with mechanisms for error reporting and handling, potentially through additional tags or flags.
* **Stricter Overlap Prevention:** More rigorous checks or constraints could be enforced during the creation and manipulation of handover records to prevent overlaps.
