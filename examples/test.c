#define CCOVER_IMAGE_IMPLEMENTATION
#include "ccover.h"

int main(int argc, char *argv[])
{
    if (argc == 3)
        GetImagePro(argv[1], argv[2]);
    else if (argc == 2)
        GetImage(argv[1]);
    else
        printf("Please Give a mp3 file first\n");
    return 0;
}