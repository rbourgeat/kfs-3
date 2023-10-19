/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 12:47:47 by rbourgea          #+#    #+#             */
/*   Updated: 2023/10/19 15:18:39 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"

void init_paging() {
	uint32_t *page_directory = &page_directory_first_entry;
	//set each entry to not present
	for(int i = 0; i < 1024; i++) {
		// This sets the following flags to the pages:
		//   Supervisor: Only kernel-mode can access them
		//   Write Enabled: It can be both read from and written to
		//   Not Present: The page table is not present
		page_directory[i] = 0x00000002;
	}

	page_directory[1023] = (uint32_t)page_directory | 3;

	//we will fill all 1024 entries in the table, mapping 4 megabytes
	uint32_t *first_page_table = &page_table_first_entry;
	for(int i = 0; i < 1024; i++)
	{
		// As the address is page aligned, it will always leave 12 bits zeroed.
		// Those bits are used by the attributes ;)
		first_page_table[i] = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present.
	}

	page_directory[0] = (uint32_t)first_page_table | 3;
	page_directory[768] = (uint32_t)first_page_table | 3;
	
	loadPageDirectory(page_directory);
	enablePaging();
}
