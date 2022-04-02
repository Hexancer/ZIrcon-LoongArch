#include "asm.h"
#include "setjmp_impl.h"
#include <zircon/tls.h>

ENTRY(setjmp)
    break 0
END(setjmp)

ALIAS(setjmp, _setjmp)
WEAK_ALIAS(setjmp, sigsetjmp)
