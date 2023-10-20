/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 16:33:57 by rbourgea          #+#    #+#             */
/*   Updated: 2023/10/20 16:47:27 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"

uint32_t	align(uint32_t addr, uint32_t boundary)
{
	return (((addr + (boundary - 1)) / boundary) * boundary);
}

unsigned char*	get_bitmask()
{
	return (bitmask);
}

uint32_t	get_bitmask_size()
{
	return (bm_size);
}

void	*get_heap_start()
{
	return (heap_start);
}

void	*get_heap_end()
{
	return (heap_end);
}

/*
	the first 256 MB of physical memory is mapped to 
	the virtual address range 0xC0000000 - 0xD0000000 (HIGH_MEM). 
	tables are created for the range 0xD0000000 - 0xE0000000 (VMALLOC)
*/

static void	intital_map()
{
	uint32_t	*pde = (uint32_t *)0xFFFFF000;
	uint32_t	frames = 0;

	for (int i = 768; i < 896; i++, tables += 1024) {
		for (int j = 0; j < 1024; j++, frames += PAGE_SIZE) {
			if (i < 832)
				tables[j] = frames | 3;
			else
				tables[j] = 0;
		}
		pde[i] = ((uint32_t)tables - PAGE_OFFSET) | 3;
	}
	refresh_map();
}

void	init_memory() {
	uint32_t frames_in_use;

	bm_size = align(MAX_ADDR, PAGE_SIZE) / PAGE_SIZE / 8;
	frames_in_use = PHISYCAL_KE / PAGE_SIZE + align(bm_size, PAGE_SIZE) / PAGE_SIZE;
	tables = (uint32_t *)(frames_in_use * PAGE_SIZE + PAGE_OFFSET);
	heap_start = (void *)((frames_in_use + 128) * PAGE_SIZE + PAGE_OFFSET);
	heap_end = (void *)0xD0000000;
	intital_map();
	kbzero(bitmask, bm_size);
	for (uint32_t i = 0; i < frames_in_use + 128 ; i++) {
		bitmask[i / 8] = bitmask[i / 8] | (1 << (i % 8));
	}
}
