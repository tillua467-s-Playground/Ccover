/* ccover - A cover extraction single header libary in C*/

/*
    Do this:
        #define CCOVER_IMAGE_IMPLEMENTATION
    before you include this file in *one* C file to create the implementation.
*/

#ifndef CCOVER_H
#define CCOVER_H

enum FORMAT
{
    PNG = 1,
    JPG
};

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    bool HasApic(char *fileDir);
    enum FORMAT CheckFormat(char *fileDir);

#ifdef __cplusplus
}
#endif

#endif /* CCOVER_H */

#ifdef CCOVER_IMAGE_IMPLEMENTATION

static unsigned char ccover__APIC_SPEC[] = {0x41, 0x50, 0x49, 0x43};
static unsigned char ccover__PNG_SPEC[] = {0x69, 0x6D, 0x61, 0x67, 0x65, 0x2F, 0x70, 0x6E, 0x67};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool HasApic(char *fileDir)
{
    // reset
    bool hasapic = false;

    FILE *f = fopen(fileDir, "rb");

    fseek(f, 0L, SEEK_END);
    long fsz = ftell(f);
    rewind(f);

    char *buffbin = (char *)malloc(fsz);
    fread(buffbin, 1, fsz, f);
    int apicSz = sizeof(ccover__APIC_SPEC);

    for (long i = 0; i <= fsz - apicSz; i++)
    {
        if (memcmp(buffbin + i, ccover__APIC_SPEC, apicSz) == 0)
        {
            hasapic = true;
            break;
        }
    }

    free(buffbin);
    fclose(f);
    return hasapic;
}

enum FORMAT CheckFormat(char *fileDir)
{
    if (HasApic(fileDir))
    {
        bool isPng, isJpg;

        FILE *f = fopen(fileDir, "rb");

        fseek(f, 0L, SEEK_END);
        long fsz = ftell(f);
        rewind(f);

        char *buffBin = (char *)malloc(fsz);
        fread(buffBin, 1, fsz, f);
        int PngSize = sizeof(ccover__PNG_SPEC);
        for (long i = 0; i < fsz - PngSize; i++)
        {
            if (memcmp(buffBin + i, ccover__PNG_SPEC, PngSize) == 0)
            {
                isPng = true;
                break;
            }
        }

        if (isPng)
            return PNG;
        else
            return JPG;
    }
    else
    {
        printf("Error: File doesn't have Attached Picture\n");
    }
}

#endif /* CCOVER_IMAGE_IMPLEMENTATION */
