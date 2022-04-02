#pragma once

#include <lib/zircon-internal/device/cpu-trace/perf-mon.h>
#include <lib/zircon-internal/device/cpu-trace/common-pm.h>

// TODO: Complete PMU

#define LOONGARCH64_PMU_MAX_PROGRAMMABLE_COUNTERS (6u)
#define LOONGARCH64_PMU_MAX_FIXED_COUNTERS (1u)

///////////////////////////////////////////////////////////////////////////////

namespace perfmon {

// These structs are used for communication between the device driver
// and the kernel.

// Properties of perf data collection on this system.
struct Loongarch64PmuProperties : public PmuCommonProperties {
};

// Configuration data passed from driver to kernel.
struct Loongarch64PmuConfig {
    // The id of the timebase counter to use or |kEventIdNone|.
    // A "timebase counter" is used to trigger collection of data from other
    // events. In other words it sets the sample rate for those events.
    // If zero, then no timebase is in use: Each event must trigger its own
    // data collection. Otherwise the value is the id of the timebase counter
    // to use, which must appear in one of |programmable_ids| or |fixed_ids|.
    PmuEventId timebase_event;

    // Ids of each event. These values are written to the trace buffer to
    // identify the event.
    // The used entries begin at index zero and are consecutive (no holes).
    PmuEventId fixed_events[LOONGARCH64_PMU_MAX_FIXED_COUNTERS];
    PmuEventId programmable_events[LOONGARCH64_PMU_MAX_PROGRAMMABLE_COUNTERS];

    // Initial value of each counter.
    uint64_t fixed_initial_value[LOONGARCH64_PMU_MAX_FIXED_COUNTERS];
    uint32_t programmable_initial_value[LOONGARCH64_PMU_MAX_PROGRAMMABLE_COUNTERS];

    // Flags for each counter.
    // The values are |perfmon::kPmuConfigFlag*|.
    uint32_t fixed_flags[LOONGARCH64_PMU_MAX_FIXED_COUNTERS];
    uint32_t programmable_flags[LOONGARCH64_PMU_MAX_PROGRAMMABLE_COUNTERS];

    // H/W event numbers, one entry for each element in |programmable_events|.
    // There is only one fixed event, the cycle counter, so we don't need to
    // record its event number here.
    uint32_t programmable_hw_events[LOONGARCH64_PMU_MAX_PROGRAMMABLE_COUNTERS];
};

} // namespace perfmon