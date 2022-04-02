#include "asm.h"
#include "setjmp_impl.h"
#include <zircon/tls.h>

ENTRY(setjmp)

END(setjmp)

ALIAS(setjmp, _setjmp)
WEAK_ALIAS(setjmp, sigsetjmp)
