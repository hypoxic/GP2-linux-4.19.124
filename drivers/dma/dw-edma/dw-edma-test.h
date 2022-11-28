// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright 2020 Socionext, Inc.
 * eDMA client test sample
 *
 * Author: Taichi Sugaya <sugaya.taichi@socionext.com>
 */

#include <linux/pci.h>

#ifndef _DW_EDMA_TEST_H
#define _DW_EDMA_TEST_H

int dw_edma_test(struct pci_dev *pdev);

#endif
