/* libunwind - a platform-independent unwind library
   Copyright (C) 2008 CodeSourcery
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

PROTECTED int
unw_is_greg (int regnum)
{
  TODO();
  return (regnum >= UNW_LOONGARCH64_R0 && regnum <= UNW_LOONGARCH64_PC);
}

PROTECTED int
unw_is_fpreg (int regnum)
{
  TODO();
  return (regnum >= UNW_LOONGARCH64_F0 && regnum <= UNW_LOONGARCH64_FCSR);
}

HIDDEN int
tdep_access_reg (struct cursor *c, unw_regnum_t reg, unw_word_t *valp,
                 int write)
{
  TODO();
  dwarf_loc_t loc = DWARF_NULL_LOC;
  unsigned int mask;

  switch (reg)
    {
    case UNW_LOONGARCH64_R4:
    case UNW_LOONGARCH64_R5:
    case UNW_LOONGARCH64_R6:
    case UNW_LOONGARCH64_R7:

      mask = 1 << reg;
      if (write)
        {
          c->dwarf.eh_args[reg] = *valp;
          c->dwarf.eh_valid_mask |= mask;
          return 0;
        }
      else if ((c->dwarf.eh_valid_mask & mask) != 0)
        {
          *valp = c->dwarf.eh_args[reg];
          return 0;
        }
      else
        loc = c->dwarf.loc[reg];
      break;

    case UNW_LOONGARCH64_R0:
    case UNW_LOONGARCH64_R1:
    case UNW_LOONGARCH64_R2:
    case UNW_LOONGARCH64_R3:

    case UNW_LOONGARCH64_R8:
    case UNW_LOONGARCH64_R9:
    case UNW_LOONGARCH64_R10:
    case UNW_LOONGARCH64_R11:
    case UNW_LOONGARCH64_R12:
    case UNW_LOONGARCH64_R13:
    case UNW_LOONGARCH64_R14:
    case UNW_LOONGARCH64_R15:
    case UNW_LOONGARCH64_R16:
    case UNW_LOONGARCH64_R17:
    case UNW_LOONGARCH64_R18:
    case UNW_LOONGARCH64_R19:
    case UNW_LOONGARCH64_R20:
    case UNW_LOONGARCH64_R21:
    case UNW_LOONGARCH64_R22:
    case UNW_LOONGARCH64_R23:
    case UNW_LOONGARCH64_R24:
    case UNW_LOONGARCH64_R25:
    case UNW_LOONGARCH64_R26:
    case UNW_LOONGARCH64_R27:
    case UNW_LOONGARCH64_R28:
    case UNW_LOONGARCH64_R29:
    case UNW_LOONGARCH64_R30:
    case UNW_LOONGARCH64_PC:
      loc = c->dwarf.loc[reg];
      break;

    default:
      Debug (1, "bad register number %u\n", reg);
      return -UNW_EBADREG;
    }

  if (write)
    return dwarf_put (&c->dwarf, loc, *valp);
  else
    return dwarf_get (&c->dwarf, loc, valp);
}

HIDDEN int
tdep_access_fpreg (struct cursor *c, unw_regnum_t reg, unw_fpreg_t *valp,
                   int write)
{
  Debug (1, "bad register number %u\n", reg);
  return -UNW_EBADREG;
}
