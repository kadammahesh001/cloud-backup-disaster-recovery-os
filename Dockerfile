FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    nasm \
    grub-pc-bin \
    grub-common \
    xorriso \
    qemu-system-x86 \
    gcc-i686-linux-gnu \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /kernel

COPY . .

CMD ["make", "run"]