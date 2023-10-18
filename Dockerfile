FROM	debian:latest

RUN	apt-get	update
RUN	apt-get	install build-essential xorriso nasm -y
RUN	apt-get	install grub-pc-bin -y

WORKDIR	/kfs
