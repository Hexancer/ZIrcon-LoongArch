/* libunwind - a platform-independent unwind library
   Copyright (C) 2012 Tommi Rantala <tt.rantala@gmail.com>
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

#include <zircon/assert.h>

#include "unwind_i.h"
#include "dwarf_i.h"

static inline int
common_init (struct cursor *c, unsigned use_prev_instr)
{
  TODO();
  int ret;

  c->dwarf.loc[UNW_LOONGARCH64_R0]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R0);
  c->dwarf.loc[UNW_LOONGARCH64_R1]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R1);
  c->dwarf.loc[UNW_LOONGARCH64_R2]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R2);
  c->dwarf.loc[UNW_LOONGARCH64_R3]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R3);
  c->dwarf.loc[UNW_LOONGARCH64_R4]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R4);
  c->dwarf.loc[UNW_LOONGARCH64_R5]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R5);
  c->dwarf.loc[UNW_LOONGARCH64_R6]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R6);
  c->dwarf.loc[UNW_LOONGARCH64_R7]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R7);
  c->dwarf.loc[UNW_LOONGARCH64_R8]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R8);
  c->dwarf.loc[UNW_LOONGARCH64_R9]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R9);
  c->dwarf.loc[UNW_LOONGARCH64_R10] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R10);
  c->dwarf.loc[UNW_LOONGARCH64_R11] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R11);
  c->dwarf.loc[UNW_LOONGARCH64_R12] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R12);
  c->dwarf.loc[UNW_LOONGARCH64_R13] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R13);
  c->dwarf.loc[UNW_LOONGARCH64_R14] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R14);
  c->dwarf.loc[UNW_LOONGARCH64_R15] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R15);
  c->dwarf.loc[UNW_LOONGARCH64_R16] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R16);
  c->dwarf.loc[UNW_LOONGARCH64_R17] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R17);
  c->dwarf.loc[UNW_LOONGARCH64_R18] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R18);
  c->dwarf.loc[UNW_LOONGARCH64_R19] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R19);
  c->dwarf.loc[UNW_LOONGARCH64_R20] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R20);
  c->dwarf.loc[UNW_LOONGARCH64_R21] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R21);
  c->dwarf.loc[UNW_LOONGARCH64_R22] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R22);
  c->dwarf.loc[UNW_LOONGARCH64_R23] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R23);
  c->dwarf.loc[UNW_LOONGARCH64_R24] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R24);
  c->dwarf.loc[UNW_LOONGARCH64_R25] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R25);
  c->dwarf.loc[UNW_LOONGARCH64_R26] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R26);
  c->dwarf.loc[UNW_LOONGARCH64_R27] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R27);
  c->dwarf.loc[UNW_LOONGARCH64_R28] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R28);
  c->dwarf.loc[UNW_LOONGARCH64_R29] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R29);
  c->dwarf.loc[UNW_LOONGARCH64_R30] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R30);
  c->dwarf.loc[UNW_LOONGARCH64_R31] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_R31);

  c->dwarf.loc[UNW_LOONGARCH64_F0]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F0);
  c->dwarf.loc[UNW_LOONGARCH64_F1]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F1);
  c->dwarf.loc[UNW_LOONGARCH64_F2]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F2);
  c->dwarf.loc[UNW_LOONGARCH64_F3]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F3);
  c->dwarf.loc[UNW_LOONGARCH64_F4]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F4);
  c->dwarf.loc[UNW_LOONGARCH64_F5]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F5);
  c->dwarf.loc[UNW_LOONGARCH64_F6]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F6);
  c->dwarf.loc[UNW_LOONGARCH64_F7]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F7);
  c->dwarf.loc[UNW_LOONGARCH64_F8]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F8);
  c->dwarf.loc[UNW_LOONGARCH64_F9]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F9);
  c->dwarf.loc[UNW_LOONGARCH64_F10] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F10);
  c->dwarf.loc[UNW_LOONGARCH64_F11] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F11);
  c->dwarf.loc[UNW_LOONGARCH64_F12] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F12);
  c->dwarf.loc[UNW_LOONGARCH64_F13] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F13);
  c->dwarf.loc[UNW_LOONGARCH64_F14] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F14);
  c->dwarf.loc[UNW_LOONGARCH64_F15] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F15);
  c->dwarf.loc[UNW_LOONGARCH64_F16] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F16);
  c->dwarf.loc[UNW_LOONGARCH64_F17] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F17);
  c->dwarf.loc[UNW_LOONGARCH64_F18] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F18);
  c->dwarf.loc[UNW_LOONGARCH64_F19] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F19);
  c->dwarf.loc[UNW_LOONGARCH64_F20] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F20);
  c->dwarf.loc[UNW_LOONGARCH64_F21] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F21);
  c->dwarf.loc[UNW_LOONGARCH64_F22] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F22);
  c->dwarf.loc[UNW_LOONGARCH64_F23] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F23);
  c->dwarf.loc[UNW_LOONGARCH64_F24] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F24);
  c->dwarf.loc[UNW_LOONGARCH64_F25] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F25);
  c->dwarf.loc[UNW_LOONGARCH64_F26] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F26);
  c->dwarf.loc[UNW_LOONGARCH64_F27] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F27);
  c->dwarf.loc[UNW_LOONGARCH64_F28] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F28);
  c->dwarf.loc[UNW_LOONGARCH64_F29] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F29);
  c->dwarf.loc[UNW_LOONGARCH64_F30] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F30);
  c->dwarf.loc[UNW_LOONGARCH64_F31] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_F31);

  c->dwarf.loc[UNW_LOONGARCH64_PC]  = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_PC);
  c->dwarf.loc[UNW_LOONGARCH64_FCSR] = DWARF_REG_LOC (&c->dwarf, UNW_LOONGARCH64_FCSR);

  // TODO: Anything else here?s

  ret = dwarf_get (&c->dwarf, c->dwarf.loc[UNW_LOONGARCH64_PC], &c->dwarf.ip);
  if (ret < 0)
    return ret;

  ret = dwarf_get (&c->dwarf, c->dwarf.loc[UNW_LOONGARCH64_R3], &c->dwarf.cfa);
  if (ret < 0)
    return ret;

  c->sigcontext_format = LOONGARCH64_SCF_NONE;
  c->sigcontext_addr = 0;

  c->dwarf.args_size = 0;
  c->dwarf.ret_addr_column = 0;
  c->dwarf.stash_frames = 0;
  c->dwarf.use_prev_instr = use_prev_instr;
  c->dwarf.pi_valid = 0;
  c->dwarf.pi_is_dynamic = 0;
  c->dwarf.hint = 0;
  c->dwarf.prev_rs = 0;

  return 0;
}
