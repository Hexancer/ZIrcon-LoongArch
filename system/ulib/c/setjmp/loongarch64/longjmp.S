#include "asm.h"
#include "setjmp_impl.h"
#include <zircon/tls.h>

ENTRY(longjmp)
    break 0
END(longjmp)

ALIAS(longjmp, _longjmp)
WEAK_ALIAS(longjmp, siglongjmp)
