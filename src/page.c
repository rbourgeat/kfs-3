/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   page.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/19 16:14:56 by rbourgea          #+#    #+#             */
/*   Updated: 2023/10/20 16:48:01 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"

static void	*heap_start;
static void	*heap_end;

static void	get_page_init()
{
	heap_start = get_heap_start();
	heap_end = get_heap_end();
}

static void	*get_page_in_lowmem(uint32_t nbr)
{
	void	*frames;

	if (!heap_start)
		get_page_init();
	frames = get_frames(heap_start - PAGE_OFFSET, heap_end - PAGE_OFFSET, nbr);
	if (frames)
		return (frames + PAGE_OFFSET);
	return (NULL);
}

static void free_page_in_lowmem(void *page, uint32_t nbr)
{
	for (uint32_t i = 0; i < nbr; i++)
	{
		free_frame(page - PAGE_OFFSET);
		page = page + PAGE_SIZE;
	}
}

void *get_page(uint32_t flags, uint32_t nbr)
{
	if (flags == LOW_MEM)
		return (get_page_in_lowmem(nbr));
	return (NULL);
}
