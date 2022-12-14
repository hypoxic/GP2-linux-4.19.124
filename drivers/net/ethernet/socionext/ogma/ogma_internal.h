/**
 * ogma_itnernal.h
 *
 *  Copyright (c) 2015 SOCIONEXT INCORPORATED.
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *   
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 *
 */
#ifndef OGMA_INTERNAL_H
#define OGMA_INTERNAL_H
#include "ogma_api.h"
#include "ogma_reg.h"
#include "pfdep.h"

/* Just a million to prevent typing errors. */
#define OGMA_CLK_MHZ (1000000)

#define OGMA_INSTANCE_NUM_MAX 1

#define OGMA_RX_PKT_BUF_LEN 1522
#define OGMA_RX_JUMBO_PKT_BUF_LEN 9022
#define OGMA_DUMMY_DESC_ENTRY_LEN 48

extern const ogma_uint32 desc_ring_irq_inten_reg_addr[OGMA_DESC_RING_ID_MAX +
						      1];
extern const ogma_uint32 desc_ring_irq_inten_set_reg_addr[OGMA_DESC_RING_ID_MAX
							  + 1];
extern const ogma_uint32 desc_ring_irq_inten_clr_reg_addr[OGMA_DESC_RING_ID_MAX
							  + 1];

typedef struct ogma_ctrl_s ogma_ctrl_t;
typedef struct ogma_desc_ring_s ogma_desc_ring_t;
typedef struct ogma_clk_ctrl_s ogma_clk_ctrl_t;
typedef union ogma_desc_entry_priv_u ogma_desc_entry_priv_t;

struct ogma_clk_ctrl_s {
	ogma_uint32 dmac_req_num;
	ogma_uint32 core_req_num;
	ogma_uint8 mac_req_num;
};

struct ogma_desc_ring_s {

	ogma_desc_ring_id_t ring_id;

	ogma_desc_ring_param_t param;

	ogma_uint rx_desc_ring_flag:1;

	ogma_uint tx_desc_ring_flag:1;

	ogma_uint running_flag:1;

	ogma_uint full_flag:1;

	ogma_uint8 desc_entry_len;

	ogma_uint16 head_idx;

	ogma_uint16 tail_idx;

	ogma_uint16 rx_num;

	ogma_uint16 tx_done_num;

	pfdep_hard_lock_t inten_reg_hard_lock;

	pfdep_soft_lock_t soft_lock;

	void *desc_ring_cpu_addr;

	pfdep_phys_addr_t desc_ring_phys_addr;

	ogma_frag_info_t *frag_info_p;

	ogma_desc_entry_priv_t *priv_data_p;
};

struct ogma_ctrl_s {
	ogma_ctrl_t *next_p;

	ogma_uint core_enabled_flag:1;

	ogma_uint gmac_rx_running_flag:1;

	ogma_uint gmac_tx_running_flag:1;

	ogma_uint gmac_mode_valid_flag:1;

	void *base_addr;

	pfdep_dev_handle_t dev_handle;

	pfdep_netdev_handle_t netdev_handle;

	ogma_param_t param;

	ogma_clk_ctrl_t clk_ctrl;

	ogma_uint32 rx_pkt_buf_len;

	ogma_desc_ring_t desc_ring[OGMA_DESC_RING_ID_MAX + 1];

	ogma_gmac_mode_t gmac_mode;

	pfdep_hard_lock_t inten_reg_hard_lock;

	pfdep_hard_lock_t clk_ctrl_hard_lock;

	void *dummy_desc_entry_addr;

	pfdep_phys_addr_t dummy_desc_entry_phys_addr;

	void *netsec_handle;
};

union ogma_desc_entry_priv_u {
	pfdep_pkt_handle_t pkt_handle;
};

/*****************************************************************
 *****************************************************************
 *****************************************************************/

/**
 *
 */
ogma_err_t ogma_alloc_desc_ring(ogma_ctrl_t * ctrl_p,
				ogma_desc_ring_id_t ring_idx);
/**
 *
 */
void ogma_free_desc_ring(ogma_ctrl_t * ctrl_p, ogma_desc_ring_t * desc_ring_p);

/**
 *
 */
ogma_err_t ogma_setup_rx_desc_ring(ogma_ctrl_t * ctrl_p,
				   ogma_desc_ring_t * desc_ring_p);

/**
 *
 */
void ogma_uninit_pkt_desc_ring(ogma_ctrl_t * ctrl_p,
			       ogma_desc_ring_t * desc_ring_p);

/**
 *
 */
void ogma_push_clk_req(ogma_ctrl_t * ctrl_p, ogma_uint32 domain);

void ogma_pop_clk_req(ogma_ctrl_t * ctrl_p, ogma_uint32 domain);

#ifdef OGMA_CONFIG_CHECK_CLK_SUPPLY
#include "ogma_basic_access.h"
/*include for ogma_read_reg*/
static __inline void ogma_check_clk_supply(ogma_ctrl_t * ctrl_p,
					   ogma_uint32 domain)
{
	ogma_uint32 value;

	value = ogma_read_reg(ctrl_p, OGMA_REG_ADDR_CLK_EN);

	pfdep_assert(((value & domain) == domain));
}

#else

#define ogma_check_clk_supply( ctrl_p, domain)

#endif				/* OGMA_CONFIG_CHECK_CLK_SUPPLY */

#endif				/* OGMA_INTERNAL_H */
