/* ccover - A cover extraction single header libary in C*/

/*
    Do this:
        #define CCOVER_IMAGE_IMPLEMENTATION
    before you include this file in *one* C file to create the implementation.
*/

#ifndef CCOVER_H
#define CCOVER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    int HasApic(char *fileDir);

#ifdef __cplusplus
}
#endif

#endif /* CCOVER_H */

#ifdef CCOVER_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char ccover__APIC_SPEC[] = {0x41, 0x50, 0x49, 0x43};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int HasApic(char *fileDir)
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

#endif /* CCOVER_IMAGE_IMPLEMENTATION */