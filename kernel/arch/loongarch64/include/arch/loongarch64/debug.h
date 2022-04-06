#pragma once

#include <arch/loongarch64/debug.h>

extern int laprintf(const char *fmt, ...);

#define DEBUG(cond, fmt, ...) \
    if (cond) laprintf("[%s:%d %s] " fmt "\n", __FILE__, __LINE__, __func__, ## __VA_ARGS__)
