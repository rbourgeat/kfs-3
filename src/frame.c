/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   frame.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/20 15:25:57 by rbourgea          #+#    #+#             */
/*   Updated: 2023/10/20 17:28:46 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"

static unsigned char	*bitmask;
static uint32_t			bm_size;
static uint32_t			max_pfn;

static void init_frame_allocator()
{
	bitmask = get_bitmask();
	bm_size = get_bitmask_size();
	max_pfn = MAX_ADDR / PAGE_SIZE;
	printk("MAX_ADDR: %d", MAX_ADDR);
}

static void pfn_to_idxs(uint32_t pfn, uint32_t *i, uint32_t *j)
{
	*i = pfn / 8;
	*j = pfn % 8;
}

void free_frame(void* addr)
{
	uint32_t pfn = (uint32_t)addr / PAGE_SIZE;
	uint32_t i;
	uint32_t j;

	pfn_to_idxs(pfn, &i, &j);
	bitmask[i] &= ~(1 << j); // Clear the bit
}

static int is_in_range(uint32_t end_frame, uint32_t i, uint32_t j)
{
	uint32_t pfn = i * 8 + j;

	return (pfn < max_pfn && pfn < end_frame);
}

static void *mark_frames_occupied(uint32_t i, int j, uint32_t frames)
{
	void* ret_value = (void *)(PAGE_SIZE * (i * 8 + j));
	uint32_t nbr = 0;

	for (; nbr < frames; i++) {
		for (; j < 8 && nbr < frames; j++) {
			bitmask[i] |= (1 << j); // Set the bit
			nbr++;
		}
		j = 0;
	}
	return (ret_value);
}

static void *get_contiguous_frames(uint32_t end_frame, uint32_t *i, uint32_t *j, uint32_t frames) {
	uint32_t nbr = 0;
	uint32_t i_begin = *i;
	uint32_t j_begin = *j;

	for (; is_in_range(end_frame, *i, *j); (*i)++) {
		for (; is_in_range(end_frame, *i, *j); (*j)++) {
			if (!(bitmask[*i] & (1 << *j)))
				nbr++;
			else
				return (NULL);
			if (nbr == frames)
				return mark_frames_occupied(i_begin, j_begin, frames);
		}
		*j = 0;
	}
	return (NULL);
}

void *get_frames(void *start, void *end, uint32_t frames)
{
	void *ret_value = NULL;
	uint32_t i;
	uint32_t j;
	uint32_t start_frame;
	uint32_t end_frame;

	start_frame = (uint32_t)start / PAGE_SIZE;
	end_frame = (uint32_t)end / PAGE_SIZE;

	if (!bitmask) {
		init_frame_allocator();
	}

	pfn_to_idxs(start_frame, &i, &j);
	for (; is_in_range(end_frame, i, j) && !ret_value; i++) {
		if (bitmask[i] == 0xFF)
			continue;
		for (; is_in_range(end_frame, i, j) && !ret_value; j++) {
			if (!(bitmask[i] & (1 << j))) {
				ret_value = get_contiguous_frames(end_frame, &i, &j, frames);
			}
		}
		j = 0;
	}
	return (ret_value);
}
