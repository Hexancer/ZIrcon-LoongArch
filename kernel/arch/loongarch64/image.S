#include <asm.h>
#include <zircon/boot/image.h>

.text

// ZBI file header (zbi_header_t)
ZBI_CONTAINER_HEADER(_zbi_file_header, boot_load_end - _zbi_kernel_header)

// ZBI kernel header (zbi_header_t)
DATA(_zbi_kernel_header)
    .int ZBI_TYPE_KERNEL_LOONGARCH64
    .int boot_load_end - _zbi_kernel_payload
    .int 0
    .int ZBI_FLAG_VERSION
    .int 0
    .int 0
    .int ZBI_ITEM_MAGIC
    .int ZBI_ITEM_NO_CRC32
END_DATA(_zbi_kernel_header)

// ZBI_TYPE_KERNEL payload (zbi_kernel_t)
DATA(_zbi_kernel_payload)
// The boot-shim code expects this to be an offset from the beginning
// of the load image, whatever the kernel's virtual address.
.quad IMAGE_ELF_ENTRY - _zbi_file_header
.quad IMAGE_MEMORY_END - boot_load_end
END_DATA(_zbi_kernel_payload)

// Pad out to the header size that was allocated in the kernel image layout.
// This ensures that the kernel image is aligned correctly in memory.
.org BOOT_HEADER_SIZE

// Include the kernel image itself, skipping the padding left for the headers.
DATA(kernel_image)
#include "kernel_image.h"
.incbin KERNEL_IMAGE, BOOT_HEADER_SIZE
DATA(kernel_image_end)
END_DATA(kernel_image)

.balign 8
DATA(boot_load_end)

.macro fixup addr, n, stride
.endm
#include "kernel-fixups.inc"