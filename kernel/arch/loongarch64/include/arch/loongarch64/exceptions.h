#pragma once

// Flags passed back from loongarch64_irq() to the calling assembler.
#define LOONGARCH64_IRQ_EXIT_THREAD_SIGNALED 1
#define LOONGARCH64_IRQ_EXIT_RESCHEDULE 2
