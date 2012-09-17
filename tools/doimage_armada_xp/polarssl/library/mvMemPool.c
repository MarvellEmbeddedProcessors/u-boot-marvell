/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
        used to endorse or promote products derived from this software without
        specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "polarssl/mvMemPool.h"


/*******************************************************************************
*		mpool_get_free_node()
*******************************************************************************/
static pool_node * mpool_get_free_node (pool_head *head)
{
	if (head != 0) {
		register pool_node *node = head->free_list;

		if (node != 0) {
			/* remove first element from the free list */
			head->free_list = node->next;
			/* is it the last free element? */
			if (head->free_list != 0) {
				head->free_list->prev = 0;
			}

			/* put into the busy list */
			node->next = head->busy_list;
			node->prev = 0;
			head->busy_list = node;
			if (node->next != 0) {
				node->next->prev = node;
			}

#ifdef MV_MEMPOOL_STAT
			head->stat.alloc_req++;
			head->stat.allocated++;
			if (head->stat.allocated > head->stat.max_alloc)
				head->stat.max_alloc = head->stat.allocated;
#endif

			return node;
		}
	}

	return 0;
} /* end of get_free_node() */

/*******************************************************************************
*		mpool_free_busy_node()
*******************************************************************************/
static void mpool_free_busy_node (pool_node *node)
{
	if (node != 0) {
		register pool_head *head = node->head;

#ifdef MV_MEMPOOL_STAT
		head->stat.free_req++;
		head->stat.allocated--;
#endif

		/* remove the node from the busy list */
		if (node->next != 0) {
			node->next->prev = node->prev;
		}
		if (node->prev != 0) {
			node->prev->next = node->next;
		}
		if ((node->next == 0) && (node->prev == 0)) {
			head->busy_list = 0;
		}

		/* connect the node to the free list */
		node->next = head->free_list;
		node->prev = 0;
		head->free_list = node;
		if (node->next != 0) {
			node->next->prev = node;
		}
	}
} /* end of free_busy_node() */

/*******************************************************************************
*		mpool_get_heads()
*******************************************************************************/
pool_head * mpool_get_heads (void)
{
	static pool_head heads[NUM_OF_POOLS];

	return heads;
} /* end of mpool_get_heads */

/*******************************************************************************
*		mpool_init()
*******************************************************************************/
pool_head * mpool_init (void)
{
	register int	pn, nn; /* pool number, node number */

	int pool_data_sizes[NUM_OF_POOLS] = {POOL_DATA_SZ_1, POOL_DATA_SZ_2,
										 POOL_DATA_SZ_3, POOL_DATA_SZ_4};

	static unsigned char pool_data_1 [NUM_OF_NODES][POOL_DATA_SZ_1] = {0};
	static unsigned char pool_data_2 [NUM_OF_NODES][POOL_DATA_SZ_2] = {0};
	static unsigned char pool_data_3 [NUM_OF_NODES][POOL_DATA_SZ_3] = {0};
	static unsigned char pool_data_4 [NUM_OF_NODES][POOL_DATA_SZ_4] = {0};

	pool_head *heads = mpool_get_heads();

	memset(heads, 0, NUM_OF_POOLS * sizeof(pool_head));

	for (pn = 0; pn < NUM_OF_POOLS; pn++) {

		heads[pn].busy_list = 0;
		heads[pn].data_sz   = pool_data_sizes[pn];

		for (nn = 0; nn < NUM_OF_NODES; nn++) {
			pool_node	*pooln;

			switch (pn) {
				case 0:
					pooln = (pool_node *)&pool_data_1[nn][0];
					break;
				case 1:
					pooln = (pool_node *)&pool_data_2[nn][0];
					break;
				case 2:
					pooln = (pool_node *)&pool_data_3[nn][0];
					break;
				case 3:
					pooln = (pool_node *)&pool_data_4[nn][0];
					break;
				default:
					return NULL;
			}

			/* last node? */
			if (nn == NUM_OF_NODES - 1) {
				pooln->next = 0;
			} else {
				pooln->next =
					(pool_node *)((unsigned char *)pooln + heads[pn].data_sz);
			}
			/* first node? */
			if (nn == 0) {
				pooln->prev = 0;
				heads[pn].free_list = pooln;
			} else {
				pooln->prev =
					(pool_node *)((unsigned char *)pooln - heads[pn].data_sz);
			}

			pooln->head = &heads[pn];

		} /* for all nodes */
	} /* for all pools */

	return heads;
} /* end of mpool_init() */

/*******************************************************************************
*		mpool_alloc()
*******************************************************************************/
void * mpool_alloc (pool_head *head, int size)
{
	register int	   pn;
	register pool_node *node;

	for (pn = 0; pn < NUM_OF_POOLS; pn++) {
		if (MAX_DATA_SZ(&head[pn]) >= size) {
			node = mpool_get_free_node(&head[pn]);
			if (node != 0) {
				memset(&node->data, 0, MAX_DATA_SZ(node->head));
				return &node->data;
			}
		}
	}

	return 0;
}/* end of mpool_alloc() */

/*******************************************************************************
*		mpool_grow()
*******************************************************************************/
void * mpool_grow (pool_head *head, void *mr, int new_size)
{
	if (mr == 0) {
		/* No data to copy, just allocate it */
		return mpool_alloc(head, new_size);
	} else {
		register pool_node *node =
				(pool_node *)((unsigned char *)mr - offsetof(pool_node, data));

		if (MAX_DATA_SZ(node->head) >= new_size) {
			/* nothing to do, the current memory region is bigh enough */
			return mr;
		} else {
			register void *new_data = mpool_alloc(head, new_size);

			if (new_data != 0) {
				memcpy(new_data, &node->data, MAX_DATA_SZ(node->head));
				memset(&node->data, 0, MAX_DATA_SZ(node->head));
				mpool_free_busy_node(node);
			}
			return new_data;
		}
	}
}/* end of mpool_grow() */

/*******************************************************************************
*		mpool_free()
*******************************************************************************/
void mpool_free (void *mr)
{
	register pool_node *node;

	node = (pool_node *)((unsigned char *)mr - offsetof(pool_node, data));
	memset(&node->data, 0, MAX_DATA_SZ(node->head));

	mpool_free_busy_node(node);
}/* end of mpool_free() */

#ifdef MV_MEMPOOL_STAT
/*******************************************************************************
*		mpool_print_stat()
*******************************************************************************/
void mpool_print_stat (void)
{
	pool_head 	*heads = mpool_get_heads();
	int			i;

	for (i = 0; i < NUM_OF_POOLS; i++)
	{
		printf("Pool sz %d : Allocated: %d; Total alloc %d / free %d; Max alloc %d\n",
			   heads[i].data_sz,  heads[i].stat.allocated, heads[i].stat.alloc_req,
			   heads[i].stat.free_req, heads[i].stat.max_alloc);
	}
}/* end of mpool_print_stat() */

/*******************************************************************************
*		mpool_reset_stat()
*******************************************************************************/
void mpool_reset_stat (void)
{
	pool_head 	*heads = mpool_get_heads();
	int			i;

	for (i = 0; i < NUM_OF_POOLS; i++)
	{
		memset(&heads[i].stat, 0, sizeof(pool_stat));
	}
}/* end of mpool_reset_stat() */

#endif
