// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright 2020 Socionext, Inc.
 * eDMA client test sample
 *
 * Author: Taichi Sugaya <sugaya.taichi@socionext.com>
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/dma/edma.h>

struct dw_edma_test_param {
	struct device *dev;
	u32 direction;
};

static bool dw_filter(struct dma_chan *chan, void *param)
{
	struct dw_edma_test_param *tparam = param;

	if ((chan->device->directions == tparam->direction) &&
	    (chan->device->dev == tparam->dev)) {
		return true;
	} else {
		return false;
	}
}

static void dw_edma_test_callback(void *arg)
{
	struct pci_dev *pdev = arg;
	pci_info(pdev, "edma running test done\n");
}

/* test client */
int dw_edma_test(struct pci_dev *pdev)
{
	struct dma_chan *chan;
	dma_cap_mask_t mask;
	struct scatterlist sg[1];
	struct dma_slave_config conf;
	struct dma_async_tx_descriptor *desc;
	struct dw_edma_test_param tparam;
	phys_addr_t src_addr, dst_addr;
	u32 trans_size, dir;

	src_addr = 0x4a0000000;
	dst_addr = 0x5cafe0000;
	trans_size = 0x1000;
	dir = DMA_MEM_TO_DEV;	// write (RC to EP)

	if (dir == DMA_MEM_TO_DEV) {
		conf.dst_addr = dst_addr;
		sg[0].dma_address = src_addr;
	} else if (dir == DMA_DEV_TO_MEM) {
		conf.src_addr = src_addr;
		sg[0].dma_address = dst_addr;
	} else {
		pci_err(pdev, "Not support direciton\n");
		return -EINVAL;
	}
	sg[0].dma_length = trans_size;

	dma_cap_zero(mask);
	dma_cap_set(DMA_SLAVE, mask);
	tparam.dev = &pdev->dev;
	tparam.direction = BIT(dir);
	chan = dma_request_channel(mask, dw_filter, &tparam);
	if (!chan) {
		pci_err(pdev, "can't find a suitable DMA channel\n");
		return -EINVAL;
	}

	dmaengine_slave_config(chan, &conf);
	desc = dmaengine_prep_slave_sg(chan, &sg[0], ARRAY_SIZE(sg),
				       dir, DMA_PREP_INTERRUPT);
	if (!desc) {
		pci_err(pdev, "NULL descriptor!\n");
		return -EINVAL;
	}
	desc->callback = dw_edma_test_callback;
	desc->callback_param = pdev;
	dmaengine_submit(desc);
	dma_async_issue_pending(chan);
	return 0;
}
EXPORT_SYMBOL_GPL(dw_edma_test);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Synopsys DesignWare eDMA test driver");
MODULE_AUTHOR("Taichi Sugaya <sugaya.taichi@socionext.com>");
