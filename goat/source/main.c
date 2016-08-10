#include <sys/stat.h>
#include <stdio.h>

#include <3ds.h>

int main(int argc, char **argv) {
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	struct stat statbuf;
	if (stat(argv[0], &statbuf)) {
		puts("A fatal error occurred getting file information.");
		goto loop;
	}

	printf("Goat File size is:   %ld bytes\n"
		   "The current size is: %ld bytes\n",
			GOAT_SIZE, statbuf.st_size);

	loop:
	puts("\nPress START to exit.\n");
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