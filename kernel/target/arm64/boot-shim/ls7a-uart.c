// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>
#include "debug.h"

#define UART_LSR_TFE (1 << 5)

static volatile uint8_t* uart_reg_dat = (uint8_t *)0x800000001fe001e0;          // Data register 0x1fe001e0
static volatile uint8_t* uart_reg_lsr = (uint8_t *)(0x800000001fe001e0 + 0x05); // Line status register

void uart_pputc(char c)
{
    // TODO: this line spins if TFE bit is cleared,
    //   in this way we need to wait for TX FIFO to be empty before sending data,
    //   can we improve it?
    while ((*uart_reg_lsr & UART_LSR_TFE) == 0) ;//wait for fifo to be empty

    *uart_reg_dat = c;//sending data
}
