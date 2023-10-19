bits 32

global page_directory_first_entry:data
global page_table_first_entry:data

section .multiboot
	dd 0x1BADB002	; Magic number
	dd 0x0			; Flags
	dd - (0x1BADB002 + 0x0)	; Checksum

section .bootstrap.bss nobits alloc noexec write align=4
	align 16
	resb 16384					;; temp stack
	global stack_top:data
	stack_top:
	global max_addr:data
	max_addr:
	resb 4

section .tables nobits alloc noexec write align=4096
page_directory_first_entry:
	resb 4096
page_table_first_entry:
	resb 4096

global stack_bottom
global stack_top

extern gdt_desc

section .text

; .s functions
global start
global print_char_with_asm
global load_idt
global keyboard_handler
global ioport_in
global ioport_out
global enable_interrupts
global load_gdt
global loadPageDirectory
global enablePaging
global refresh_map

; .c functions
extern kmain
extern handle_keyboard_interrupt

refresh_map:
	mov ecx, cr3
	mov cr3, ecx
	ret

enablePaging:
	push ebp
	mov ebp, esp
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	mov esp, ebp
	pop ebp
	ret

loadPageDirectory:
	push ebp
	mov ebp, esp
	mov eax, [esp + 8]
	mov cr3, eax
	mov esp, ebp
	pop ebp
	ret

; Global Descriptor Table
load_gdt:
	mov eax, [esp + 4]
	lgdt [eax]

	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov ss, ax
  
	mov ax, 0x18
	mov gs, ax

	jmp 0x08:.flush
.flush:
	ret

; Interrupt Descriptor Table
load_idt:
	mov edx, [esp + 4]
	lidt [edx]
	ret

enable_interrupts:
	sti
	ret

keyboard_handler:
	pushad
	cld
	call handle_keyboard_interrupt
	popad
	iretd

ioport_in:
	mov edx, [esp + 4] ; PORT_TO_READ, 16 bits
	; dx is lower 16 bits of edx. al is lower 8 bits of eax
	; Format: in <DESTINATION_REGISTER>, <PORT_TO_READ>
	in al, dx			; Read from port DX. Store value in AL
					; Return will send back the value in eax
					; (al in this case since return type is char, 8 bits)
	ret

ioport_out:
	mov edx, [esp + 4]		; port to write; DST_IO_PORT. 16 bits
	mov eax, [esp + 8] 		; value to write. 8 bits
					; Format: out <DST_IO_PORT>, <VALUE_TO_WRITE>
	out dx, al
	ret

print_char_with_asm:
	; OFFSET = (ROW * 80) + COL
	mov eax, [esp + 8] 		; eax = row
	mov edx, 80				; 80 (number of cols per row)
	mul edx					; now eax = row * 80
	add eax, [esp + 12]		; now eax = row * 80 + col
	mov edx, 2				; * 2 because 2 bytes per char on screen
	mul edx
	mov edx, 0xb8000		; vid mem start in edx
	add edx, eax			; Add our calculated offset
	mov eax, [esp + 4] 		; char c
	mov [edx], al
	ret

start:
	mov esp, stack_space	; set stack pointer
	cli						; Disable interrupts
	mov esp, stack_space
	call kmain
	hlt

section .bss
resb 8192			; 8KB for stack
stack_space:
