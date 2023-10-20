# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/04/24 16:00:27 by rbourgea          #+#    #+#              #
#    Updated: 2023/10/20 15:31:21 by rbourgea         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
# 🎨 COLORS
# **************************************************************************** #

GREY    	=	\033[030m
RED     	=	\033[031m
GREEN   	=	\033[032m
YELLOW  	=	\033[033m
BLUE    	=	\033[034m
MAGENTA 	=	\033[035m
CYAN		=	\033[036m
BOLD		=	\033[1m
RESET   	=	\033[0m

# **************************************************************************** #
# 💾 VARIABLES
# **************************************************************************** #

KERNEL_OUT	=	build/rbourgea_kfs.bin
ISO_OUT		=	build/rbourgea_kfs.iso

BOOT		=	src/boot.s
SRC_PATH	=	src/
SRC_NAME	=	kernel.c libk.c keyboard.c prompt.c gdt.c paging.c memory.c page.c frame.c kmalloc.c
SRC			=	$(addprefix $(SRC_PATH), $(SRC_NAME))
OBJ			=	boot.o kernel.o libk.o keyboard.o prompt.o gdt.o paging.o memory.o page.o frame.o kmalloc.o
LINKER		=	src/linker.ld

FLAGS		=	-fno-builtin -fno-builtin -fno-builtin -nostdlib -nodefaultlibs

# **************************************************************************** #
# 📖 RULES
# **************************************************************************** #

all: build

build: fclean
	@mkdir -p build
	@nasm -f elf32 ${BOOT} -o boot.o
	@gcc -m32 -ffreestanding ${FLAGS} -c ${SRC}
	@echo "$(BOLD)$(GREEN)[✓] KERNEL BUILD DONE$(RESET)"
	@ld -m elf_i386 -T ${LINKER} -o ${KERNEL_OUT} ${OBJ}
	@echo "$(BOLD)$(GREEN)[✓] KERNEL LINK DONE$(RESET)"

build_debug: fclean
	@echo "$(BOLD)$(YELLOW)[✓] KERNEL DEBUG MODE ON$(RESET)"
	@mkdir -p build
	@nasm -f elf32 ${BOOT} -o boot.o
	@gcc -m32 -ffreestanding ${FLAGS} ${SRC} -ggdb
	@echo "$(BOLD)$(GREEN)[✓] KERNEL BUILD DONE$(RESET)"
	@ld -m elf_i386 -T ${LINKER} -o ${KERNEL_OUT} ${OBJ}
	@echo "$(BOLD)$(GREEN)[✓] KERNEL LINK DONE$(RESET)"

run: build
	@qemu-system-i386 -kernel ${KERNEL_OUT} -monitor stdio
	@echo "\n$(BOLD)$(CYAN)[✓] KERNEL EXIT DONE$(RESET)"

# run: docker-run

debug: build_debug
	@qemu-system-i386 -kernel ${KERNEL_OUT} -s -S &
	@gdb -x .gdbinit
	@echo "\n$(BOLD)$(CYAN)[✓] KERNEL DEBUG EXIT DONE$(RESET)"

iso: build
	@mkdir -p build/iso/boot/grub
	@cp grub.cfg build/iso/boot/grub
	@cp ${KERNEL_OUT} build/iso/boot
	@grub-mkrescue -o ${ISO_OUT} build/iso
	@echo "$(BOLD)$(GREEN)[✓] KERNEL ISO BUILD$(RESET)"

run-iso: iso
	@qemu-system-i386 -cdrom ${ISO_OUT}
	@echo "\n$(BOLD)$(CYAN)[✓] KERNEL EXIT DONE$(RESET)"

docker-run:
	clear
	@echo "\n$(BOLD)$(CYAN)[✓] STARTING DOCKER COMPILATION MODE$(RESET)"
	@echo "$(BOLD)$(RED)[♻︎] DELETING CONTAINER...$(RESET)"
	@-docker stop rbourgea-kfs
	@-docker rm rbourgea-kfs
	@docker build --platform linux/amd64 -t rbourgea-kfs .
	@docker run -d --name rbourgea-kfs --rm -i -t rbourgea-kfs
	@docker cp src/. rbourgea-kfs:/kfs
	@docker cp grub.cfg rbourgea-kfs:/kfs
	@docker exec -t rbourgea-kfs nasm -f elf32 boot.s -o boot.o
	@docker exec -t rbourgea-kfs gcc -m32 -ffreestanding ${FLAGS} -c ${SRC_NAME}
	@docker exec -t rbourgea-kfs ld -m elf_i386 -T linker.ld -o rbourgea_kfs.bin ${OBJ}
	docker exec -t rbourgea-kfs mkdir -p iso/boot/grub
	docker exec -t rbourgea-kfs mv grub.cfg iso/boot/grub
	docker exec -t rbourgea-kfs mv rbourgea_kfs.bin iso/boot
	docker exec -t rbourgea-kfs grub-mkrescue -o rbourgea_kfs.iso ./iso/
	docker cp rbourgea-kfs:/kfs/rbourgea_kfs.iso boot/rbourgea_kfs.iso
	qemu-system-i386 -cdrom boot/rbourgea_kfs.iso
#	@docker cp rbourgea-kfs:/kfs/rbourgea_kfs.bin boot/rbourgea_kfs.bin
#	@qemu-system-i386 -kernel boot/rbourgea_kfs.bin -monitor stdio
	@echo "\n$(BOLD)$(CYAN)[✓] KERNEL EXIT DONE$(RESET)"

clean:
	@rm -rf $(KERNEL_OUT) $(ISO_OUT) *.o
	@echo "$(BOLD)$(RED)[♻︎] DELETE KERNEL DONE$(RESET)"

fclean: clean
	clear
	@rm -rf build/
	@echo "$(BOLD)$(RED)[♻︎] DELETE BUILD/ DONE$(RESET)"

re: fclean all

.PHONY: all clean fclean re
