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

    bool CheckFileExist(char *fileDir);
    bool HasApic(char *fileDir);
    enum FORMAT CheckFormat(char *fileDir);
    int GetImagePro(char *mp3FileDir, char *sourceFileDir);
    void GetImage(char *fileDir);
#ifdef __cplusplus
}
#endif

#endif /* CCOVER_H */

#ifdef CCOVER_IMAGE_IMPLEMENTATION

static unsigned char ccover__APIC_SPEC[] = {0x41, 0x50, 0x49, 0x43};
static unsigned char ccover__PNG_SPEC[] = {0x69, 0x6D, 0x61, 0x67, 0x65, 0x2F, 0x70, 0x6E, 0x67};
static unsigned char ccover__pngStart[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
static unsigned char ccover__pngEnd[] = {0xAE, 0x42, 0x60, 0x82};
static unsigned char ccover__jpgStart[] = {0xFF, 0xD8};
static unsigned char ccover__jpgEnd[] = {0xFF, 0xD9};

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>

bool CheckFileExist(char *fileDir)
{
    struct stat buffer;
    return (stat(fileDir, &buffer) == 0);
}

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

int GetImagePro(char *mp3FileDir, char *sourceFileDir)
{
    FILE *f = fopen(mp3FileDir, "rb");
    fseek(f, 0L, SEEK_END);
    long fsz = ftell(f);
    rewind(f);
    unsigned char *coverbin;

    long startIndex = -1;
    long EndIndex = -1;

    char *buffBin = (char *)malloc(fsz);
    fread(buffBin, 1, fsz, f);

    if (CheckFormat(mp3FileDir) == PNG)
    {
        for (long i = 0; i < fsz; i++)
        {
            if (startIndex == -1 && memcmp(buffBin + i, ccover__pngStart, sizeof(ccover__pngStart)) == 0)
                startIndex = i;
            if (EndIndex == -1 && memcmp(buffBin + i, ccover__pngEnd, sizeof(ccover__pngEnd)) == 0)
                EndIndex = i + sizeof(ccover__pngEnd);
        }

        if (startIndex != -1 && EndIndex != -1)
        {
            long len = EndIndex - startIndex;
            coverbin = malloc(len);
            memcpy(coverbin, buffBin + startIndex, len);
            FILE *srcF = fopen(sourceFileDir, "wb+");
            fwrite(coverbin, len, 1, srcF);
            free(coverbin);
            fclose(srcF);
            fclose(f);

            return 0;
        }
    }
    else if (CheckFormat(mp3FileDir) == JPG)
    {
        for (long i = 0; i < fsz; i++)
        {
            if (startIndex == -1 && memcmp(buffBin + i, ccover__jpgStart, sizeof(ccover__jpgStart)) == 0)
                startIndex = i;
            if (EndIndex == -1 && memcmp(buffBin + i, ccover__jpgEnd, sizeof(ccover__jpgEnd)) == 0)
                EndIndex = i + sizeof(ccover__jpgEnd);
        }

        if (startIndex != -1 && EndIndex != -1)
        {
            long len = EndIndex - startIndex;
            coverbin = malloc(len);
            memcpy(coverbin, buffBin + startIndex, len);
            FILE *srcF = fopen(sourceFileDir, "wb+");
            fwrite(coverbin, len, 1, srcF);
            free(coverbin);
            fclose(srcF);
            fclose(f);
            return 0;
        }
    }
    else
    {
        printf("No Valid Image Found in the file\n");
    }
}

void GetImage(char *fileDir)
{
    if (CheckFileExist(fileDir))
    {
        printf("File Already exist\n");
        exit(1);
    }
    if (CheckFormat(fileDir) == PNG)
        GetImagePro(fileDir, "Image.png");
    else if (CheckFormat(fileDir) == PNG)
        GetImagePro(fileDir, "Image.jpg");
    else
        printf("No Valid Image Found in the file\n");
}
#endif /* CCOVER_IMAGE_IMPLEMENTATION */