#pragma once

#include <inttypes.h>

#include <arch/loongarch64/iframe.h>

// Flags passed back from loongarch64_handle_irq() to the calling assembler.
#define LOONGARCH64_IRQ_EXIT_THREAD_SIGNALED 1
#define LOONGARCH64_IRQ_EXIT_RESCHEDULE 2

extern "C" void loongarch64_handle_pf(iframe_t* iframe);
extern "C" uint32_t loongarch64_handle_irq(iframe_t* iframe);