/* Copyright (c) 2014 Tom Li
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Credits:
 *
 * MasterChef:  
 * http://www.insanelymac.com/forum/index.php?showtopic=154145
 * 
 * DieBuche: 
 * http://www.insanelymac.com/forum/topic/211294-information-on-vesa-modes-in-atinvidia-bios/
 */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <getopt.h>


/* This is always the first entry of the VESA table in NVIDIA's ROM */
static const unsigned char NV_MAGIC_VESA_CODE[30] = {
    0x00, 0x04, 0xFF, 0x03, 0x00, 0x00, 0xFF, 0x03, 
    0x17, 0x04, 0x9F, 0x04, 0x3F, 0x05, 0x00, 0x03,
    0xFF, 0x02, 0x00, 0x00, 0xFF, 0x02, 0x02, 0x03,
    0x08, 0x03, 0x25, 0x03, 0x00, 0xFF
};


/* If it is the first byte of the VESA table */
bool meet_vesa_table(unsigned char *vbios)
{
    for (int i = 0; i < 30; i++) {
        if (vbios[i] != NV_MAGIC_VESA_CODE[i]) {
            return false;
        }
    }
    return true;
}


void show_usage(char *name)
{
    printf("Usage: %s [OPTION] [NVIDIA VBIOS]\n", name);
    printf("   -h, --help\tThis help message\n");
    printf("   -v, --verbose\tShow verbose VESA modelines\n");
}


int main(int argc, char **argv)
{
    static struct option longopts[] = {
        {"help", no_argument, 0, 'h'},
        {"verbose", no_argument, 0, 'v'}
    };

    char *name = argv[0];
    bool verbose = false;
    char *filename = NULL;

    int opt;
    while ((opt = getopt_long(argc, argv, "vh", longopts, NULL)) != -1) {
        switch (opt) {
            case 'v':
                verbose = true;
                break;
            case 'h':
                show_usage(name);
                exit(0);
            case '?':
                printf("%s: invalid option -- %c\n", name, optopt);
                printf("See %s -h for usage.\n", name);
                break;
        }
    }

    if (optind < argc) {
        filename = argv[optind];
    }
    else {
        show_usage(name);
        exit(1);
    }

    puts("Warning: This is free software; see the source for copying conditions.");
    puts("There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");

    printf("Opening VBIOS: %s\n", filename);

    FILE *vbios_fp = fopen(filename, "rb");
    if (!vbios_fp) {
        printf("%s: can't open %s\n", name, filename);
        return 1;
    }

    fseek(vbios_fp, 0, SEEK_END);
    size_t length = ftell(vbios_fp);
    fseek(vbios_fp, 0, SEEK_SET);
    printf("Length: %ld\n", length);

    unsigned char *vbios = malloc(sizeof(unsigned char) * length);
    if (!vbios) {
        printf("%s: out of memory!\n", name);
        return 1;
    }
    size_t size = fread(vbios, sizeof(unsigned char), length, vbios_fp);
    fclose(vbios_fp);
    if (size != length) {
        printf("%s: incomplete read occred during fread()\n", name);
        return 1;
    }

    int vesa_tables = 0;
    for (size_t i = 0; i < length - 30; i++) {
        if (meet_vesa_table(&vbios[i])) {
            vesa_tables++;
            printf("\n#%d possible VESA modes table @ 0x%08lx\n", vesa_tables, i);
            for (size_t j = i; j < i + 512; j += 32) {
                if (vbios[j] == 0x00 && vbios[j + 1] == 0x00) {
                    printf("--- an empty modeline @ 0x%08lx\n", j);
                }
                else {
                    printf("--- ");
                    /* resolution is at the 1st and 15th byte,
                     * in uint16, little endian */
                    printf("%dx%d ", *(uint16_t *)&vbios[j], *(uint16_t *)&vbios[j + 14]);
                    if (verbose) {
                        /* print the hex dump of the modeline */
                        for (size_t z = j; z < j + 32; z++) {
                            printf("%02x ", vbios[z]);
                        }
                    }
                    printf("@ 0x%08lx\n", j);
                }
            }
        }
    }
    free(vbios);

    putchar('\n');
    if (vesa_tables == 1) {
        puts("[+] We found 1 VESA table, which is the correct number.");
        puts("[+] VBIOS Resolution modding should be working. Happy Hacking!");
    }
    else {
        printf("[!] Unexpected, we found %d VESA tables. What's wrong?\n",  vesa_tables);
        puts("[!] VBIOS Resolution modding may not working.");
    }
    return 0;
}
