OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
ENTRY(start)

SECTIONS
{
	. = 1M;

	.bootstrap.data ALIGN(4K) : {
		*(.multiboot)
		*(.bootstrap.data)
	}

	.bootstrap.bss ALIGN(4k) : {
		*(.tables)
		*(.bootstrap.bss)
	}

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}

	.data BLOCK(4K) : ALIGN(4K) 
	{ 
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(.bss)
	}

	. = ALIGN(4K);
	PROVIDE(kernel_end = .);
}
