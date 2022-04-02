#include "asm.h"
#include "setjmp_impl.h"
#include <zircon/tls.h>

ENTRY(longjmp)

END(longjmp)

ALIAS(longjmp, _longjmp)
WEAK_ALIAS(longjmp, siglongjmp)
