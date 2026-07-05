/* ccover - A cover extraction single header libary in C*/
//
// To instantiate the implementation,
//      #define CCOVER_IMAGE_IMPLEMENTATION
// in *ONE* source file, before #including this file.
//
//      just drag and drop this file in your project to get started
//
// Status:
//     - haven't tested compiling as C++
//     - only extarcts the first APIC image
//
//      LICENSE
//      See end of file for license information.
#ifndef CCOVER_H
#define CCOVER_H

#include <stdio.h>    // for fprintf
#include <stdlib.h>   // malloc(), free
#include <sys/stat.h> // file stats
#include <string.h>   // memcmp()
#include <stdbool.h>  // booleans

enum FORMAT // APIC picture extnetions
{
    PNG = 1,
    JPG
};

#ifdef __cplusplus
extern "C"
{
#endif
    // this function checks if the input file exist
    //      input:
    //          - fileDir is the path of a file
    //      output:
    //          - returns true if exist
    //          - returns false if doesn't exist
    bool CheckFileExist(char *fileDir);
    // this function checks if the input file has APIC (Attached Picture)
    //      input:
    //          - fileDir is the path of a file
    //      output:
    //          - returns true if exist
    //          - returns false if doesn't exist
    bool HasApic(char *fileDir);
    // this function checks what the input file's APIC image has extention is (in mp3 there can only be either png or jpeg image as a format)
    //      input:
    //          - fileDir is the path of the mp3 file
    //      output:
    //          - returns the extention of the Attached Picture (png or jpg)
    enum FORMAT CheckFormat(char *fileDir);
    // this function extract the image from the mp3 to a desired location
    //      input:
    //          - mp3FileDir is the path of the mp3 file
    //          - sourceFileDir is the path of the extracted image
    //      output:
    //          - returns 0 upon success
    //          - returns 1 upon failiure
    //      warning:
    //          - must be with extention eg GetImagePro("here/here/here/music.mp3", "here/here/here/Image.png/.jpg") .png or .jpg must be set
    //          - if sourceFileDir exist it will be overwritten
    int GetImagePro(char *mp3FileDir, char *sourceFileDir);
    // this function extract the image from the mp3 to the local directory
    //      input:
    //          - fileDir is the path of the mp3 file
    //      output:
    //          - returns 0 upon success
    //          - returns 1 upon failiure
    //      warning:
    //          - if sourceFileDir exist it won't let it be overwritten and return 1
    void GetImage(char *fileDir);
#ifdef __cplusplus
}
#endif

#endif /* CCOVER_H */

#ifdef CCOVER_IMAGE_IMPLEMENTATION
// --BEGIN--
// ID3V2 HEADER (more info: https://en.wikipedia.org/wiki/ID3)
// TIT(Title)
// ...OTHER STUFFS...
// APIC <- Raw binary image data
// |--> APIC(in Hex)..Size..PictureType(either image/png or image/jpeg)..png/jpgStart..png/jpegEnd
static unsigned char ccover__APIC_SPEC[] = {0x41, 0x50, 0x49, 0x43};                              //  APIC(Attached Picture) in hex
static unsigned char ccover__PNG_SPEC[] = {0x69, 0x6D, 0x61, 0x67, 0x65, 0x2F, 0x70, 0x6E, 0x67}; // image/png
static unsigned char ccover__pngStart[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
static unsigned char ccover__pngEnd[] = {0xAE, 0x42, 0x60, 0x82};
static unsigned char ccover__jpgStart[] = {0xFF, 0xD8};
static unsigned char ccover__jpgEnd[] = {0xFF, 0xD9};

bool CheckFileExist(char *fileDir)
{
    struct stat buffer;
    return (stat(fileDir, &buffer) == 0);
}

bool HasApic(char *fileDir)
{
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
        fprintf(stderr, "File doesn't have Attached Picture\n");
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
        fprintf(stderr, "No Valid Image Found in the file\n");
    }
}

void GetImage(char *fileDir)
{
    if (CheckFormat(fileDir) == PNG)
    {
        if (CheckFileExist("image.png"))
        {
            fprintf(stderr, "File Already exist\n");
            exit(1);
        }
        GetImagePro(fileDir, "Image.png");
    }
    else if (CheckFormat(fileDir) == JPG)
    {
        if (CheckFileExist("image.png"))
        {
            fprintf(stderr, "File Already exist\n");
            exit(1);
        }
        GetImagePro(fileDir, "Image.jpg");
    }
    else
        fprintf(stderr, "No Valid Image Found in the file\n");
}
#endif /* CCOVER_IMAGE_IMPLEMENTATION */

/*
This software is under MIT license
Copyright (c) 2026 Tillua467
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/