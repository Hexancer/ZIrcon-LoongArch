#ifndef LOONGARCH64_LIBUNWIND_I_H
#define LOONGARCH64_LIBUNWIND_I_H

/* Target-dependent definitions that are internal to libunwind but need
   to be shared with target-independent code.  */

#include "elf64.h"

typedef enum
  {
    UNW_LOONGARCH64_FRAME_STANDARD = -2,  /* regular fp, sp +/- offset */
    UNW_LOONGARCH64_FRAME_SIGRETURN = -1, /* special sigreturn frame */
    UNW_LOONGARCH64_FRAME_OTHER = 0,      /* not cacheable (special or unrecognised) */
    UNW_LOONGARCH64_FRAME_GUESSED = 1     /* guessed it was regular, but not known */
  }
unw_tdep_frame_type_t;

typedef enum
  {
    LOONGARCH64_SCF_NONE,
    LOONGARCH64_SCF_LINUX_RT_SIGFRAME,
  }
unw_tdep_sigcontext_format_t;

#endif /* LOONGARCH64_LIBUNWIND_I_H */
