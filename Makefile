GDB_EXEC_OPTS = -ex "cd out/" \
		-ex "set output-radix 16" \
		-ex "set architecture Loongarch64" \
                -ex "target remote localhost:1234"
		

build: config
	cd out && ../buildtools/ninja

config:
	buildtools/gn gen --export-compile-commands=x64 out

run:
	./scripts/run-zircon-x64 -z ./out/legacy-image-x64.zbi -t ./out/multiboot.bin

runarm:
	./scripts/run-zircon-arm64 -z ./out/legacy-image-arm64.zbi -t ./out/qemu-boot-shim.bin

runla64:
	./scripts/run-zircon-loongarch64 -z ./out/legacy-image-loongarch64.zbi -t ./out/ls7a-boot-shim.elf

gdbsla64:
	./scripts/run-zircon-loongarch64 -z ./out/legacy-image-loongarch64.zbi -t ./out/ls7a-boot-shim.elf --debugger

gdbla64shim:
	gdb $(GDB_EXEC_OPTS) ./out/ls7a-boot-shim.elf

gdbla64kernel:
	gdb $(GDB_EXEC_OPTS) ./out/kernel-loongarch64-gcc/obj/kernel/zircon.elf

clean:
	buildtools/gn clean out
