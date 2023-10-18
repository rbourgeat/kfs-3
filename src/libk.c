/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libk.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/20 14:58:54 by user42            #+#    #+#             */
/*   Updated: 2022/06/25 13:36:00 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"

void    kputstr(const char* data) 
{
	for (size_t i = 0; i < kstrlen(data); i++)
		kputchar(data[i]);
}

void	kputnbr(int n)
{
	char	str[kintlen(n) + 1];

	kitoa(n, str);
	kputstr(str);
}

void	kitoa(int n, char *str)
{
	int	nb;
	int	i;
	int	len;

	nb = n;
	len = kintlen(n);
	kmemset(str, 0, len + 1);
	if (nb < 0) {
		nb = -nb;
	}
	i = len - 1;
	if (nb == 0) {
		str[i] = '0';
		return;
	}
	while (nb != 0)
	{
		str[i--] = (nb % 10) + '0';
		nb = nb / 10;
	}
	if (n < 0)
		str[i] = '-';
}

int		kintlen(int n)
{
	int	i;

	i = 1;
	if (n < 0) {
		n = -n;
		i++;
	}
	while (n >= 10)
	{
		n = n / 10;
		i++;
	}
	return (i);
}

void	*kmemset(void *b, int c, unsigned int len)
{
	unsigned int	i;

	i = 0;
	while (i < len)
	{
		((char *)(b))[i] = c;
	i++;
	}
	return b;
}

void	kputchar(char c)
{
	if (c == '\n') {
		tty_column = 0;
		tty_row++;
		if (tty_row == VGA_HEIGHT)
			terminal_initialize(-1);
		return;
	}
	ttys[tty_nb][tty_column][tty_row] = c; // save to tty
	terminal_putentryat(c, tty_color, tty_column, tty_row);
	if (++tty_column == VGA_WIDTH) {
		tty_column = 0;
		if (++tty_row == VGA_HEIGHT)
			tty_row = 0;
	}
}

void	hex_to_str(unsigned int addr, char *result, int size)
{
	int	len;
	char	base_str[16] = "0123456789abcdef";

	len = size - 1;
	kmemset(result, '0', size);
	result[size - 1] = 0;
	while (addr != 0)
	{
		result[--len] = base_str[addr % 16];
		addr = addr / 16;
	}
}

size_t	kstrlen(const char* str)
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

char	*kstrjoin(char const *s1, char const *s2)
{
	size_t	count;
	size_t	size_s1;
	char	*tab;

	count = -1;
	if (!s1 || !s2)
		return (NULL);
	size_s1 = kstrlen(s1);
	while (s1[++count])
		tab[count] = s1[count];
	count = -1;
	while (s2[++count])
		tab[size_s1 + count] = s2[count];
	tab[size_s1 + count] = '\0';
	return (tab);
}

void	printk(char *str, ...)
{
	int	*nb_args;
	char	*args;
	int	i;
	char	tmp_addr[9];
	uint8_t	zero_padding;

	nb_args = (int *)(&str);
	args = (char *)(*nb_args++);
	i = 0;
	while (args[i]) {
		zero_padding = 0;
		if (args[i] == '%') {
			i++;
			if (args[i] == '0') {
				i++;
				zero_padding = args[i] - '0';
				i++;
			}
			if (args[i] == 'c')
				kputchar(*nb_args++);
			else if (args[i] == 's')
				kputstr(*((char **)nb_args++));
			else if (args[i] == 'x') {
				kmemset(tmp_addr, 0, sizeof(tmp_addr));
				hex_to_str(*nb_args++, tmp_addr, sizeof(tmp_addr));
				kputstr(tmp_addr);
			}
			else if (args[i] == 'p') {
				kmemset(tmp_addr, 0, sizeof(tmp_addr));
				hex_to_str(*nb_args++, tmp_addr, sizeof(tmp_addr));
				kputstr("0x");
				kputstr(tmp_addr);
			}
			else if (args[i] == 'd') {
				if (zero_padding > 0) {
					while (zero_padding - kintlen(*nb_args)) {
						kputchar('0');
						zero_padding--;
					}
				}
				kputnbr(*nb_args++);
			}
			else {
				kputchar('%');
				kputchar(args[i]);
			}
		}
		else
			kputchar(args[i]);
		i++;
	}
}

int	kstrcmp(const char *s1, const char *s2)
{
	int i;

	if (!s1 || !s2)
		return (-1);
	i = 0;
	while (s1[i] == s2[i])
	{
		if (s1[i] == '\0')
			return (0);
		i++;
	}
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

void	khexdump(uint32_t addr, int limit)
{
	char *c = (char *)addr;
	char str_addr[9];
	int i;
	uint32_t previous;

	if (limit <= 0)
		return;
	for (i = 0; i < limit; i++)
	{
		if ((i % 16) == 0) // 16 = size line
		{
			if (i != 0)
			{
				previous = addr - 16;
				while (previous < addr)
				{
					if (*(char *)previous <= 32)
					{
						kcolor(VGA_COLOR_RED);
						printk("%c", '.');
					}
					else
					{
						kcolor(VGA_COLOR_GREEN);
						printk("%c", *(char *)previous);
					}
					previous++;
				}
				printk("\n");
			}
			if ((uint32_t)0x00000800 == addr)
				kcolor(VGA_COLOR_CYAN);
			else
				kcolor(VGA_COLOR_LIGHT_BLUE);
			printk("%p: ", addr);
		}
		hex_to_str((uint32_t)c[i], str_addr, 3);
		if ((uint32_t)c[i] == 0) // == 00
			kcolor(VGA_COLOR_RED);
		else
			kcolor(VGA_COLOR_GREEN);
		printk("%s ", str_addr);
		kcolor(VGA_COLOR_WHITE);
		addr++;
	}
	for (i = 0; i < ((limit % 16) * 3); i++) // last line
		printk(" ");
	if ((limit % 16) == 0)
		previous = addr - 16;
	else
		previous = addr - (limit % 16);
	while (previous < addr)
	{
		if (*(char *)previous <= 32)
		{
			kcolor(VGA_COLOR_RED);
			printk("%c", '.');
		}
		else
		{
			kcolor(VGA_COLOR_GREEN);
			printk("%c", *(char *)previous);
		}
		previous++;
	}
	printk("\n");
	kcolor(VGA_COLOR_WHITE);
}
