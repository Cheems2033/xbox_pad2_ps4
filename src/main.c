#include <stdint.h>
#include <string.h>

#define PROT_READ  1
#define PROT_WRITE 2
#define PROT_EXEC  4

extern int sceKernelUsleep(unsigned int microseconds);
extern int sceKernelMprotect(const void *addr, size_t len, int prot);

int xbox_device_connected = 0;
int xbox_data_ready = 0;

int (*original_scePadRead)(int handle, void *pData, int num) = NULL;

void apply_xbox_input_to_ps4_buffer(void* pData) {
    // Aquí va tu lógica original para mezclar los botones
}

int my_scePadRead(int handle, void *pData, int num) {
    if (!original_scePadRead) return -1;
    int ret = original_scePadRead(handle, pData, num);

    if (ret < 0) {
        sceKernelUsleep(1000); 
        return ret;
    }

    if (xbox_device_connected && xbox_data_ready) {
        apply_xbox_input_to_ps4_buffer(pData);
    }

    return ret;
}

void patch_function_jmp(uintptr_t target_addr, void* hook_func) {
    if (!target_addr) return;
    uintptr_t page_start = target_addr & ~0xFFF;
    sceKernelMprotect((void*)page_start, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC);

    uint8_t jmp_code[] = {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
    };

    *(uintptr_t*)(&jmp_code[6]) = (uintptr_t)hook_func;
    memcpy((void*)target_addr, jmp_code, sizeof(jmp_code));
    sceKernelMprotect((void*)page_start, 0x1000, PROT_READ | PROT_EXEC);
}

int module_start(size_t args, const void *argp) {
    uintptr_t scePadRead_internal = 0x8004c4ea0;
    patch_function_jmp(scePadRead_internal, my_scePadRead);
    return 0;
}

int module_stop(size_t args, const void *argp) {
    return 0;
}
