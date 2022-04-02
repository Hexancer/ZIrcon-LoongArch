#include <asm.h>
#include <arch/loongarch64/mmu.h>
#include <arch/loongarch64.h>
#include <arch/asm_macros.h>
#include <arch/defines.h>
#include <zircon/tls.h>

#ifndef __has_feature
#define __has_feature(x) 0
#endif

// This code is purely position-independent and generates no relocations
// that need boot-time fixup; gen-kaslr-fixup.sh ensures this (and would
// ignore it if this code were in .text.boot, so don't put it there).
.text
FUNCTION(_start)
    break 0
END_FUNCTION(_start)

// This symbol is used by image.S
.global IMAGE_ELF_ENTRY
IMAGE_ELF_ENTRY = _start
