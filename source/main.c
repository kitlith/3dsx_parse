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

    if (head->format_ver != 0) puts("WARNING: This may be a newer version of the format! Use information at your own risk!");
    printf("Size: 0x%04x\nRelocation Header Size: 0x%04x\nVersion: 0x%08lx\nFlags: 0x%08lx\n",
        head->header_size, head->relocation_header_size, head->format_ver, head->flags);
    printf("Code Size: 0x%08lx\nROData Size: 0x%08lx\nData Size: 0x%08lx\nBSS Size: 0x%08lx\n",
        head->code_size, head->rodata_size, head->data_size, head->bss_size);

    if (head->header_size > 0x20) {
        struct extheader *ext = (struct extheader*)(head->rest);
        printf("SMDH offset: 0x%08lx\nSMDH size: 0x%08lx\nRomFS offset: 0x%08lx\n",
            ext->smdh_offset, ext->smdh_size, ext->romfs_offset);
    }

    const char *reloc_strings[3] = {
        "Code",
        "ROData",
        "Data"
    };

    for (int iii = 0; iii < 3; ++iii) {
        // There are 3 reloaction headers, starting at an offset of the header_size.
        // This is going to descend into a madness of pointer magic, isn't it?
        struct relocation_header *reloc = (struct reloaction_header*)(buffer + head->header_size + (8 * iii));
        printf("%s reloaction header\nAbsolute: 0x%08lx\nRelative: 0x%08lx\n",
            reloc_strings[iii], reloc->absolute, reloc->relative);
    }

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
