build: config
	cd out && ../buildtools/ninja

config:
	buildtools/gn gen --export-compile-commands=x64 out

run:
	./scripts/run-zircon-x64 -z ./out/legacy-image-x64.zbi -t ./out/multiboot.bin

runarm:
	./scripts/run-zircon-arm64 -z ./out/legacy-image-arm64.zbi -t ./out/qemu-boot-shim.bin

runloongarch64:
	./scripts/run-zircon-loongarch64 -z ./out/legacy-image-loongarch64.zbi -t ./out/ls7a-boot-shim.elf

gdbsloongarch64:
	./scripts/run-zircon-loongarch64 -z ./out/legacy-image-loongarch64.zbi -t ./out/ls7a-boot-shim.elf --debugger

gdbloongarch64:
	gdb -ex "set architecture Loongarch64" -ex "target remote localhost:1234"

clean:
	buildtools/gn clean out
