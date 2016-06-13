#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <3ds.h>

#pragma pack(1)
struct header {
    char magic[4];
    u16 header_size;
    u16 relocation_header_size;
    u32 format_ver;
    u32 flags;
    u32 code_size;
    u32 rodata_size;
    u32 data_size;
    u32 bss_size;
    char rest[];
};

#pragma pack(1)
struct extheader {
    u32 smdh_offset;
    u32 smdh_size;
    u32 romfs_offset;
    char rest[];
};

#pragma pack(1)
struct relocation_header {
    u32 absolute;
    u32 relative;
    char rest[];
};

#pragma pack(1)
struct relocation {
    u16 skip;
    u16 patch;
    char rest[];
};

u8 *buffer;

int main(int argc, char *argv[]) {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    FILE *file = fopen("test.3dsx", "rb");
    if (file == NULL) goto exit;

    {
        fseek(file, 0, SEEK_END);
        off_t size = ftell(file);
        fseek(file, 0, SEEK_SET);

        buffer = malloc(size);
        if (!buffer) goto close;

        off_t bytesRead = fread(buffer, 1, size, file);
        if (size != bytesRead) goto close;
    }

    char cmpmagic[4] = {'3', 'D', 'S', 'X'};
    int result = memcmp(buffer, &cmpmagic, sizeof(char) * 4);

    if (result != 0) goto close;
    struct header *head = (struct header*)buffer;

    printf("Size: 0x%04x\nRelocation Header Size: 0x%04x\nVersion: 0x%08x\nFlags: 0x%08x\n",
        head->header_size, head->relocation_header_size, head->format_ver, head->flags);
    printf("Code Size: 0x%08x\nROData Size: 0x%08x\nData Size: 0x%08x\nBSS Size: 0x%08x\n",
        head->code_size, head->rodata_size, head->data_size, head->bss_size);

    close:
    fclose(file);

    exit:
    puts("\nPress START to exit.");
    while (aptMainLoop()) {
        hidScanInput();
        if (hidKeysDown() & KEY_START) break;

        gfxFlushBuffers();
        gfxSwapBuffers();

        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}
