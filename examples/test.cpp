#define CCOVER_IMAGE_IMPLEMENTATION
#include "../ccover.h"
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc >= 3)
    {
        GetImagePro(argv[1], argv[2]);
    }
    else if (argc == 2)
    {
        GetImage(argv[1]);
    }
    else
    {
        std::cout << "Please Give a mp3 file first" << std::endl;
    }

    return 0;
}