#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <path_to_png_file>" << std::endl;
        return 1;
    }

    const char *filename = argv[1];
    int width, height, channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

    if (!data)
    {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return 1;
    }

    std::set<uint32_t> uniqueColors;
    for (int i = 0; i < width * height * 4; i += 4)
    {
        uint32_t color = (data[i + 3] << 24) | (data[i + 2] << 16) | (data[i + 1] << 8) | data[i];
        uniqueColors.insert(color);
    }

    if (uniqueColors.size() > 256)
    {
        std::cerr << "Error: Image has more than 256 unique colors." << std::endl;
        stbi_image_free(data);
        return 1;
    }

    cout << "Image size: " << width << "x" << height << endl;
    cout << "Palette size: " << uniqueColors.size() << " colors." << endl;

    std::vector<uint32_t> palette(uniqueColors.begin(), uniqueColors.end());
    std::ofstream palFile(std::string(filename) + ".pal", std::ios::binary);
    for (uint32_t color : palette)
    {
        palFile.write(reinterpret_cast<char *>(&color), sizeof(color));
    }
    palFile.close();

    std::ofstream btmFile(std::string(filename) + ".bitmap", std::ios::binary);
    for (int i = 0; i < width * height * 4; i += 4)
    {
        uint32_t color = (data[i + 3] << 24) | (data[i + 2] << 16) | (data[i + 1] << 8) | data[i];
        uint8_t index = std::distance(palette.begin(), std::find(palette.begin(), palette.end(), color));
        btmFile.write(reinterpret_cast<char *>(&index), sizeof(index));
    }
    btmFile.close();

    stbi_image_free(data);
    return 0;
}