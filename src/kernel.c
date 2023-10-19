/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/16 16:29:20 by rbourgea          #+#    #+#             */
/*   Updated: 2023/10/19 13:05:11 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"

/* ************************************************************************** */
/* Globals                                                                    */
/* ************************************************************************** */

struct IDT_entry IDT[IDT_SIZE]; // This is our entire IDT. Room for 256 interrupts

const size_t	VGA_WIDTH = 80;
const size_t	VGA_HEIGHT = 25;
 
size_t		tty_row;
size_t		tty_column;
uint8_t		tty_color;
uint16_t*	terminal_buffer;

char		prompt_buffer[1024];
int		prompt_buffer_i;
int		tty_nb;
int		tty_pos;
int		tty_prompt_pos;
char		ttys[10][256][256];
size_t		ttys_row[10];
size_t		ttys_column[10];

/* ************************************************************************** */
/* Code                                                                       */
/* ************************************************************************** */

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

void	terminal_initialize(int init)
{
	tty_row = 0;
	tty_column = 0;
	tty_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) 
	{
		for (size_t x = 0; x < VGA_WIDTH; x++) 
		{
			const size_t index = y * VGA_WIDTH + x;
			if (init == -1) 
			{
				terminal_buffer[index] = vga_entry(' ', tty_color);
				ttys[tty_nb][x][y] = ' ';
			}
			else
				terminal_buffer[index] = vga_entry(ttys[tty_nb][x][y], tty_color);
		}
	}
}

void	kcolor(uint8_t color)
{
	tty_color = color;
}

void	terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

static int count_digits(int n)
{
	int digits;

	if (n == 0)
		return (1);
	digits = n < 0 ? 1 : 0;
	while (n != 0)
	{
		n = n / 10;
		digits++;
	}
	return (digits);
}

void	khello(void)
{
	kcolor(VGA_COLOR_LIGHT_GREEN);
	kputstr("\n 42\\   42\\  424242\\        42\\        424242\\           \n");
	kputstr(" 42 |  42 |42  __42\\       42 |      42  __42\\          \n");
	kputstr(" 42 |  42 |\\__/  42 |      42 |  42\\ 42 /  \\__|424242=\\ \n");
	kputstr(" 42424242 | 424242  |      42 | 42  |4242\\    42  _____|\n");
	kputstr(" \\_____42 |42  ____/       424242  / 42  _|   \\424242\\  \n");
	kputstr("       42 |42 |            42  _42<  42 |      \\____42\\ \n");
	kputstr("       42 |42424242\\       42 | \\42\\ 42 |     424242/  |\n");
	kputstr("       \\__|\\________|      \\__|  \\__|\\__|     \\_______/ \n\n");
	kputstr("                                           by rbourgea \2\n");
	kcolor(VGA_COLOR_LIGHT_MAGENTA);
	kputstr(" \4 Login on ttys");
	kputchar((char)(tty_nb + '0'));
	kputstr("\n");
	kcolor(VGA_COLOR_WHITE);
}

void init_colrow(void)
{
	for (int i = 0; i < 10; i++)
	{
		ttys_column[i] = -1;
		ttys_row[i] = -1;
	}
}

void	kmain()
{
	init_colrow();
	terminal_initialize(-1);
	khello();
	init_paging();
	init_memory();
	// khexdump(0x00000800, 10);
	init_gdt();
	// printk(" \11 %s, %d%c\n\n", "Hello", 42, '!');

	init_idt();
	kb_init();
	enable_interrupts();

	// khexdump(0x000007c0, 100);
	
	kprompt(0);
	while(42);
}
