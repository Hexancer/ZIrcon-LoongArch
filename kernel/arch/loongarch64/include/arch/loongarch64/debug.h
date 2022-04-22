#pragma once

#include <zircon/compiler.h>

__BEGIN_CDECLS

int laprintf(const char *fmt, ...);

#define DEBUG(cond, fmt, ...) \
    if (cond) laprintf("[%s:%d %s] " fmt "\n", __FILE__, __LINE__, __func__, ## __VA_ARGS__)


__END_CDECLS