/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/20 14:59:25 by user42            #+#    #+#             */
/*   Updated: 2023/10/20 16:47:57 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KERNEL_H
# define KERNEL_H

/* ************************************************************************** */
/* Includes                                                                   */
/* ************************************************************************** */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ************************************************************************** */
/* Pre-processor constants                                                    */
/* ************************************************************************** */

# define LINES 25 // 2 bytes each
# define COLUMNS_IN_LINE 80

# define IDT_SIZE 256 // Specific to x86 architecture
# define KERNEL_CODE_SEGMENT_OFFSET 0x8
# define IDT_INTERRUPT_GATE_32BIT 0x8e

# define PIC1_COMMAND_PORT 0x20
# define PIC1_DATA_PORT 0x21
# define PIC2_COMMAND_PORT 0xA0
# define PIC2_DATA_PORT 0xA1

# define KEYBOARD_DATA_PORT 0x60
# define KEYBOARD_STATUS_PORT 0x64

# pragma GCC diagnostic ignored "-Waddress-of-packed-member"

# define PAGE_OFFSET	0xC0000000
# define PAGE_SIZE		4096
# define LOW_MEM		0
# define MAX_ADDR *(uint32_t *)((void *)&max_addr + PAGE_OFFSET)
# define PHISYCAL_KE ((uint32_t)&kernel_end - PAGE_OFFSET)
# define MAX_SIZE_LIMIT 0x000400000

/* ************************************************************************** */
/* Structs                                                                    */
/* ************************************************************************** */

struct IDT_pointer {
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));

struct IDT_entry {
	unsigned short offset_lowerbits; // 16 bits
	unsigned short selector; // 16 bits
	unsigned char zero; // 8 bits
	unsigned char type_attr; // 8 bits
	unsigned short offset_upperbits; // 16 bits
} __attribute__((packed));

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

typedef struct s_block {
	void		*next;
	void		*prev;
	uint32_t	status;
	uint32_t	size;
} t_block;

/* ************************************************************************** */
/* Globals                                                                    */
/* ************************************************************************** */

extern struct IDT_entry IDT[IDT_SIZE]; // This is our entire IDT. Room for 256 interrupts

extern const size_t	VGA_WIDTH;
extern const size_t	VGA_HEIGHT;
 
extern size_t		tty_row;
extern size_t		tty_column;
extern uint8_t		tty_color;
extern uint16_t*	terminal_buffer;

extern char		prompt_buffer[1024];
extern int		prompt_buffer_i;
extern int		tty_nb;
extern int		tty_pos;
extern int		tty_prompt_pos;
extern char		ttys[10][256][256];
extern size_t	ttys_row[10];
extern size_t	ttys_column[10];

extern int		keycodemode;

extern unsigned char	kernel_end;
extern uint32_t			max_addr;
extern uint32_t 		page_directory_first_entry;
extern uint32_t 		page_table_first_entry;

static unsigned char	*bitmask = &kernel_end;
static uint32_t			bm_size;
static uint32_t			*tables;
static void				*heap_start;
static void				*heap_end;

static t_block *block_list;

/* ************************************************************************** */
/* boots.s functions                                                          */
/* ************************************************************************** */

extern void keyboard_handler();
extern char ioport_in(unsigned short port);
extern void print_char_with_asm(char c, int row, int col); // useless
extern void ioport_out(unsigned short port, unsigned char data);
extern void load_idt(unsigned int* idt_address);
extern void enable_interrupts();
extern void loadPageDirectory(unsigned int*);
extern void enablePaging();
extern void	refresh_map();

/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */

// libk.c
void    kputnbr(int n);
int		kintlen(int n);
void	kputchar(char c);
void	kitoa(int n, char *str);
void	kputstr(const char* data);
void	khexdump(uint32_t addr, int limit);
void*	kmemset(void *b, int c, unsigned int len);
void	hex_to_str(unsigned int addr, char *result, int size);
char*	kstrjoin(char const *s1, char const *s2);
int		kstrcmp(const char *s1, const char *s2);
size_t	kstrlen(const char* str);
void	printk(char *str, ...);
void	kbzero(void *s, size_t n);

// keyboard.c
void	kb_init();
void	init_idt();
void	handle_keyboard_interrupt();

// terminal.c
void	kprompt(char c);
void	switch_screen(int nb);
void	terminal_initialize(int init);
void	set_cursor_position(uint16_t position);

// kernel.c
void	khello(void);
void	kcolor(uint8_t color);
void    terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void	khexdump(uint32_t addr, int limit);

// gdt.c
void	init_gdt();

// paging.c
void	init_paging();

// memory.c
uint32_t	align(uint32_t addr, uint32_t boundary);
void		init_memory();
void		*get_heap_start();
void		*get_heap_end();
uint32_t	get_bitmask_size();
unsigned char*	get_bitmask();

// page.c
void	*get_page(uint32_t flags, uint32_t nbr);

// frame.c
void	*get_frames(void *start, void *end, uint32_t frames);
void	free_frame(void* addr);

// kmalloc.c
void		kfree(void *ptr);
void		*kmalloc(uint32_t size);
uint32_t	get_memory_block_size(void *ptr);

#endif