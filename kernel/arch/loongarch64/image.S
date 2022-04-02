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
