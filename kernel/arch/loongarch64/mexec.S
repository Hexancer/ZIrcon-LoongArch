#include <arch/asm_macros.h>
#include <arch/defines.h>
#include <asm.h>
#include <mexec.h>

.section .text
FUNCTION(mexec_asm)
    break 0
END_FUNCTION(mexec_asm)

DATA(mexec_asm_end)