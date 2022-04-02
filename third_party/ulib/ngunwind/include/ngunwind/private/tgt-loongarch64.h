/* libunwind - a platform-independent unwind library
   Copyright (C) 2001-2004 Hewlett-Packard Co
        Contributed by David Mosberger-Tang <davidm@hpl.hp.com>
   Copyright (C) 2013 Linaro Limited

This file is part of libunwind.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  */

#ifndef LIBUNWIND_H
#define LIBUNWIND_H

#include <stdint.h>
#include <ucontext.h>

#define UNW_TARGET      loongarch64
#define UNW_TARGET_LOONGARCH64      1

/* This needs to be big enough to accommodate "struct cursor", while
   leaving some slack for future expansion.  Changing this value will
   require recompiling all users of this library.  Stack allocation is
   relatively cheap and unwind-state copying is relatively rare, so we
   want to err on making it rather too big than too small.
   As of this writing, 256 is enough so 512 provides a fair bit of room.
   The original value of 4096 is excessive.  */
#define UNW_TDEP_CURSOR_LEN     512

typedef uintptr_t unw_word_t;
typedef intptr_t unw_sword_t;

typedef long double unw_tdep_fpreg_t;

// TODO: Figure out these regs
typedef enum
  {
    /* 64-bit general registers.  */
    UNW_LOONGARCH64_R0,
    UNW_LOONGARCH64_R1,
    UNW_LOONGARCH64_R2,
    UNW_LOONGARCH64_R3,
    UNW_LOONGARCH64_R4,
    UNW_LOONGARCH64_R5,
    UNW_LOONGARCH64_R6,
    UNW_LOONGARCH64_R7,
    UNW_LOONGARCH64_R8,
    UNW_LOONGARCH64_R9,
    UNW_LOONGARCH64_R10,
    UNW_LOONGARCH64_R11,
    UNW_LOONGARCH64_R12,
    UNW_LOONGARCH64_R13,
    UNW_LOONGARCH64_R14,
    UNW_LOONGARCH64_R15,
    UNW_LOONGARCH64_R16,
    UNW_LOONGARCH64_R17,
    UNW_LOONGARCH64_R18,
    UNW_LOONGARCH64_R19,
    UNW_LOONGARCH64_R20,
    UNW_LOONGARCH64_R21,
    UNW_LOONGARCH64_R22,
    UNW_LOONGARCH64_R23,
    UNW_LOONGARCH64_R24,
    UNW_LOONGARCH64_R25,
    UNW_LOONGARCH64_R26,
    UNW_LOONGARCH64_R27,
    UNW_LOONGARCH64_R28,
    UNW_LOONGARCH64_R29,
    UNW_LOONGARCH64_R30,
    UNW_LOONGARCH64_R31,

    UNW_LOONGARCH64_PC,

    UNW_LOONGARCH64_F0,
    UNW_LOONGARCH64_F1,
    UNW_LOONGARCH64_F2,
    UNW_LOONGARCH64_F3,
    UNW_LOONGARCH64_F4,
    UNW_LOONGARCH64_F5,
    UNW_LOONGARCH64_F6,
    UNW_LOONGARCH64_F7,
    UNW_LOONGARCH64_F8,
    UNW_LOONGARCH64_F9,
    UNW_LOONGARCH64_F10,
    UNW_LOONGARCH64_F11,
    UNW_LOONGARCH64_F12,
    UNW_LOONGARCH64_F13,
    UNW_LOONGARCH64_F14,
    UNW_LOONGARCH64_F15,
    UNW_LOONGARCH64_F16,
    UNW_LOONGARCH64_F17,
    UNW_LOONGARCH64_F18,
    UNW_LOONGARCH64_F19,
    UNW_LOONGARCH64_F20,
    UNW_LOONGARCH64_F21,
    UNW_LOONGARCH64_F22,
    UNW_LOONGARCH64_F23,
    UNW_LOONGARCH64_F24,
    UNW_LOONGARCH64_F25,
    UNW_LOONGARCH64_F26,
    UNW_LOONGARCH64_F27,
    UNW_LOONGARCH64_F28,
    UNW_LOONGARCH64_F29,
    UNW_LOONGARCH64_F30,
    UNW_LOONGARCH64_F31,

    UNW_LOONGARCH64_FCSR,

    //

    UNW_LOONGARCH64_CFA = UNW_LOONGARCH64_R3,

    UNW_TDEP_LAST_REG = UNW_LOONGARCH64_FCSR,

    UNW_TDEP_IP = UNW_LOONGARCH64_R1,
    UNW_TDEP_SP = UNW_LOONGARCH64_R3,
    UNW_TDEP_EH = UNW_LOONGARCH64_R4,
  }
loongarch64_regnum_t;

#define UNW_TDEP_NUM_EH_REGS    4
typedef ucontext_t unw_tdep_context_t;

#endif /* LIBUNWIND_H */
