#include "generalinterface.h"

Serial daplink(P2_1,P2_0);
USBSerial microUSB;
SDBlockDevice bd(P0_5, P0_6, P0_4, P0_7);
FATFileSystem fs("fs");
USBMSD_BD msd(&bd);

void clearSerialStream()
{
    char c;
    while(daplink.readable()) {
        c = daplink.getc();
        wait_ms(1);
    }
}

bool getInput(int maxSize,char *inputArray)
{
    int i;
    char c;
    clearSerialStream();
    for(i=0; i<maxSize && c!='\r'; i++) {
        c = daplink.getc();
        daplink.putc(c);
        inputArray[i] = c;
    }
    if(i == maxSize) {
        return 0;
    } else {
        return 1;
    }
}

void startFileSystem()
{
    printf("\f---STARTING FILESYSTEM...---\r\n");

    Thread::wait(100);

    // Try to mount the filesystem
    printf("Mounting the filesystem... ");
    fflush(stdout);
    int err = fs.mount(&bd);
    printf("%s\r\n", (err ? "Fail :(" : "OK"));
    if (err) {
        // Reformat if we can't mount the filesystem
        // this should only happen on the first boot
        printf("No filesystem found, formatting... ");
        fflush(stdout);
        err = fs.reformat(&bd);
        printf("%s\r\n", (err ? "Fail :(" : "OK"));
    }

    // Open the numbers file
    printf("Opening \"/fs/numbers.txt\"... ");
    fflush(stdout);
    FILE *f = fopen("/fs/numbers.txt", "r+");
    printf("%s\r\n", (!f ? "Fail :(" : "OK"));
    if (!f) {
        // Create the numbers file if it doesn't exist
        printf("No file found, creating a new file... ");
        fflush(stdout);
        f = fopen("/fs/numbers.txt", "w+");
        printf("%s\r\n", (!f ? "Fail :(" : "OK"));

        for (int i = 0; i < 10; i++) {
            printf("\rWriting numbers (%d/%d)... ", i, 10);
            fflush(stdout);
            err = fprintf(f, "    %d\r\n", i);
            if (err < 0) {
                printf("Fail :(\r\n");
            }
        }
        printf("\rWriting numbers (%d/%d)... OK\r\n", 10, 10);

        printf("Seeking file... ");
        fflush(stdout);
        err = fseek(f, 0, SEEK_SET);
        printf("%s\r\n", (err < 0 ? "Fail :(" : "OK"));
    }

    // Go through and increment the numbers
    for (int i = 0; i < 10; i++) {
        printf("\rIncrementing numbers (%d/%d)... ", i, 10);
        fflush(stdout);

        // Get current stream position
        long pos = ftell(f);

        // Parse out the number and increment
        int32_t number;
        fscanf(f, "%d", &number);
        number += 1;

        // Seek to beginning of number
        fseek(f, pos, SEEK_SET);

        // Store number
        fprintf(f, "    %d\r\n", number);
    }
    printf("\rIncrementing numbers (%d/%d)... OK\r\n", 10, 10);

    // Close the file which also flushes any cached writes
    printf("Closing \"/fs/numbers.txt\"... ");
    fflush(stdout);
    err = fclose(f);
    printf("%s\r\n", (err < 0 ? "Fail :(" : "OK"));

    // Display the root directory
    printf("Opening the root directory... ");
    fflush(stdout);
    DIR *d = opendir("/fs/");
    printf("%s\r\n", (!d ? "Fail :(" : "OK"));

    printf("root directory:\r\n");
    while (true) {
        struct dirent *e = readdir(d);
        if (!e) {
            break;
        }
        printf("    %s\r\n", e->d_name);
    }

    printf("Closing the root directory... ");
    fflush(stdout);
    err = closedir(d);
    printf("%s\r\n", (err < 0 ? "Fail :(" : "OK"));

    // Display the numbers file
    printf("Opening \"/fs/numbers.txt\"... ");
    fflush(stdout);
    f = fopen("/fs/numbers.txt", "r");
    printf("%s\r\n", (!f ? "Fail :(" : "OK"));

    printf("numbers:\r\n");
    while (!feof(f)) {
        int c = fgetc(f);
        printf("%c", c);
    }

    printf("\rClosing \"/fs/numbers.txt\"... ");
    fflush(stdout);
    err = fclose(f);
    printf("%s\r\n", (err < 0 ? "Fail :(" : "OK"));




    // Switch to MSD
//    printf("Unmounting... ");
//    fflush(stdout);
//    err = fs.unmount();
//    printf("%s\r\n", (err < 0 ? "Fail :(" : "OK"));


    printf("Starting MSD... ");
    msd.disk_initialize();
    err = msd.connect();
    printf("%s\r\n", (err < 0 ? "Fail :(" : "OK"));
}