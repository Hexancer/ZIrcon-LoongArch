#include <arch/mp.h>
#include <arch/ops.h>
#include <assert.h>
#include <err.h>
#include <kernel/event.h>
#include <platform.h>
#include <trace.h>
#include <zircon/types.h>

zx_status_t arch_mp_send_ipi(mp_ipi_target_t target, cpu_mask_t mask, mp_ipi_t ipi) {
//  LTRACEF("target %d mask %#x, ipi %d\n", target, mask, ipi);
//
//  // translate the high level target + mask mechanism into just a mask
//  switch (target) {
//  case MP_IPI_TARGET_ALL:
//    mask = (1ul << SMP_MAX_CPUS) - 1;
//    break;
//  case MP_IPI_TARGET_ALL_BUT_LOCAL:
//    mask = (1ul << SMP_MAX_CPUS) - 1;
//    mask &= ~cpu_num_to_mask(arch_curr_cpu_num());
//    break;
//  case MP_IPI_TARGET_MASK:;
//  }
//
//  return interrupt_send_ipi(mask, ipi);
  return ZX_OK;
}