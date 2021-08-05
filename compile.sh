~/riscgcc/bin/riscv64-unknown-elf-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medlow -g -ffreestanding -O1 -Wl,--gc-sections -nostartfiles -nostdlib -nodefaultlibs -Wl,-T,metal.default.lds crt0.s main.c
