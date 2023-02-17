#define _USE_MATH_DEFINES
#include <cmath>
#include "image.h"
#include <iostream>
#include <cassert>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <fstream>
#define STBI_MSC_SECURE_CRT

namespace agl {


    Image::Image() : Width(1), Height(1), Data(new unsigned char[3]), Channels_num(3), allocationFlag(0) {
    }

    Image::Image(int width, int height) : Width(width), Height(height), Data(new unsigned char[width * height * 3]), Channels_num(3), allocationFlag(0) {
    }

    Image::Image(const Image& orig) : Width(orig.width()), Height(orig.height()), Channels_num(orig.channels_num()), allocationFlag(0) {
        Data = new unsigned char[Width * Height * Channels_num];
        memcpy((void*)Data, (const void*)orig.Data, sizeof(unsigned char) * Width * Height * Channels_num);
    }

    Image& Image::operator=(const Image& orig) {
        if (&orig == this) {
            return *this;
        }
        delete[] Data;

        Width = orig.Width;
        Height = orig.Height;
        Data = new unsigned char[Width * Height * Channels_num];
        memcpy((void*)Data, (const void*)orig.Data, sizeof(unsigned char) * Width * Height * Channels_num);
        Channels_num = orig.Channels_num;
        allocationFlag = orig.allocationFlag;
        return *this;
    }

    Image::~Image() {
        (*this).cleanUp();
    }
    void Image::cleanUp() {
        if (Data == NULL) { // prevents double destructor calls
            return;
        }
        else if (allocationFlag) {
            stbi_image_free(Data);
        }
        else {
            delete[] Data;
        }
        Data = NULL;
    }

int Image::width() const { 
    return this->Width;
}

int Image::height() const {
   return this->Height;
}

unsigned char* Image::data() const {
   return this->Data;
}

int Image::channels_num() const {
    return this->Channels_num;
}

bool Image::load(const std::string& filename, bool flip) {
    (*this).cleanUp();
    Data = stbi_load(filename.c_str(), &Width, &Height, &Channels_num, 3);
    allocationFlag = 1;
    if (Data == NULL) {
        return false;
    }
    return true;
}

bool Image::save(const std::string& filename, bool flip) const {
    return stbi_write_png(filename.c_str(), width(), height(), channels_num(), data(), width() * channels_num());
}

Pixel Image::get(int row, int col) const {
    if (row * width() + col >= width() * height()) {
        std::cout << "Invalid dimensions" << std::endl;
        Pixel p;
        return p;
    }
    Pixel p;
    p.r = Data[(row * width() + col) * channels_num()];
    p.g = Data[(row * width() + col) * channels_num() + 1];
    p.b = Data[(row * width() + col) * channels_num() + 2];
    return p;
}

Pixel Image::get(int i) const
{
    if (i >= width() * height() * channels_num()) {
        std::cout << "Invalid dimensions" << std::endl;
        Pixel p;
        return p;
    }
    Pixel p;
    p.r = Data[i];
    p.g = Data[i + 1];
    p.b = Data[i + 2];
    return p;
}

void Image::set(int width, int height, unsigned char* data) {
    (*this).cleanUp();
    this->Width = width;
    this->Height = height;
    Data = new unsigned char[Width * Height * 3]; // assumed to be rgb pixels
    memcpy((void*)Data, (const void*)data, sizeof(unsigned char*) * width * height * channels_num());
}

void Image::set(int row, int col, const Pixel& color) {
    if (row * width() + col >= width() * height() + width()) {
        std::cout << "row: " << row << ", col: " << col << "Invalid dimensions" << std::endl;
        return;
    }
    Data[(row * width() + col) * channels_num()] = color.r;
    Data[(row * width() + col) * channels_num() + 1] = color.g;
    Data[(row * width() + col) * channels_num() + 2] = color.b;
}

void Image::set(int i, const Pixel& color) {
    if (i >= width() * height() * channels_num()) {
        std::cout << "Invalid dimensions" << std::endl;
        return;
    }
    Data[i * channels_num() + 1] = color.r;
    Data[i * channels_num() + 1] = color.g;
    Data[i * channels_num() + 2] = color.b;
}

Image Image::resize(int w, int h) const {
   Image result(w, h);
   for (int i = 0; i < h; ++i) {
       for (int j = 0; j < w; ++j) {
           result.Data[(i * w + j) * channels_num()] = Data[((i * (height() - 1) / (h - 1)) * width() + (j * (width() - 1) / (w - 1))) * channels_num()];
           result.Data[(i * w + j) * channels_num() + 1] = Data[((i * (height() - 1) / (h - 1)) * width() + (j * (width() - 1) / (w - 1))) * channels_num() + 1];
           result.Data[(i * w + j) * channels_num() + 2] = Data[((i * (height() - 1) / (h - 1)) * width() + (j * (width() - 1) / (w - 1))) * channels_num() + 2];
       }
   }
   return result;
}

Image Image::flipHorizontal() const {
    Image result(width(), height());
    for (int i = 0; i < height(); ++i) {
        for (int j = 0; j < width(); ++j) {
            if (i == 399 && j == 399) {
                break;
            }
            result.Data[((i * width()) + width() - j - 1) * channels_num()] = Data[(i * width() + j) * channels_num()];
            result.Data[((i * width()) + width() - j - 1) * channels_num() + 1] = Data[(i * width() + j) * channels_num() + 1];
            result.Data[((i * width()) + width()  - j - 1) * channels_num() + 2] = Data[(i * width() + j) * channels_num() + 2];
        }
    }
    return result;
}

Image Image::flipVertical() const {
   Image result(Width, Height);
   for (int i = 0; i < Height; ++i) {
       for (int j = 0; j < Width; ++j) {
           result.Data[((Height - i - 1) * width() + j) * channels_num()] = Data[(i * width() + j) * channels_num()];
           result.Data[((Height - i - 1) * width() + j) * channels_num() + 1] = Data[(i * width() + j) * channels_num() + 1];
           result.Data[((Height - i - 1) * width() + j) * channels_num() + 2] = Data[(i * width() + j) * channels_num() + 2];
       }
   }
   return result;
}

Image Image::rotate90() const {
    Image result(Width, Height);
    for (int i = 0; i < Height; ++i) {
        for (int j = 0; j < Width; ++j) {
            result.Data[(j * width() + width() - i - 1) * channels_num()] = Data[(i * width() + j) * channels_num()];
            result.Data[(j * width() + width() - i - 1) * channels_num() + 1] = Data[(i * width() + j) * channels_num() + 1];
            result.Data[(j * width() + width() - i - 1) * channels_num() + 2] = Data[(i * width() + j) * channels_num() + 2];
        }
    }
    return result;
}

Image Image::subimage(int startx, int starty, int w, int h) const {
    if (!(startx < width() && starty < height() && w <= width() && h <= height())) {
        std::cout << "illegal dimensions, subimage must be equal to or smaller than original image" << std::endl;
        return Image();
    }
    Image sub = Image(w, h);
    for (int i = 0; i < std::min(h, height()); ++i) {
         for (int j = 0; j < std::min(w, width()); ++j) {
             sub.Data[(i * w + j) * channels_num()] = Data[((starty + i) * width() + startx + j) * channels_num()];
             sub.Data[(i * w + j) * channels_num() + 1] = Data[((starty + i) * width() + startx + j) * channels_num() + 1];
             sub.Data[(i * w + j) * channels_num() + 2] = Data[((starty + i) * width() + startx + j) * channels_num() + 2];
        }
    }
    return sub;
}

void Image::replace(const Image& image, int startx, int starty) {
    Image result = Image(width(), height());
    result.Data = Data;
    for (int i = 0; i < std::min(height(), image.height()); ++i) {
         for (int j = 0; j < std::min(width(), image.width()); ++j) {
            result.Data[((startx + i) * width() + j + starty) * channels_num()] = image.Data[(i * image.width() + j) * image.channels_num()];
            result.Data[((startx + i) * width() + j + starty)* channels_num() + 1] = image.Data[(i * image.width() + j) * image.channels_num() + 1];
            result.Data[((startx + i) * width() + j + starty)* channels_num() + 2] = image.Data[(i * image.width() + j) * image.channels_num() + 2];
         }
    }
}

Image Image::swirl() const {
    Image result = Image(width(), height());
    for (int i = 0; i < width() * height() * channels_num(); ++i) {
        unsigned char red = Data[i];
        unsigned char green = Data[i + 1];
        unsigned char blue = Data[i + 2];
        result.Data[i] = green;
        result.Data[i + 1] = blue;
        result.Data[i + 2] = red;
    }
    return result;
}

Image Image::add(const Image& other) const {
   Image result(0, 0);
  
   return result;
}

Image Image::subtract(const Image& other) const {
   Image result(0, 0);
   
   return result;
}

Image Image::multiply(const Image& other) const {
    if (width() * height() != other.width() * other.height()) {
        std::cout << "images must be the same size";
        return Image();
    }
    Image result(width(), height());
    for (int i = 0; i < width() * height() * channels_num(); ++i) {
        result.Data[i] = (Data[i] / 255.0f) * (other.Data[i] / 255.0f) * 255.0f;
        result.Data[i + 1] = (Data[i + 1] / 255.0f) * (other.Data[i + 1] / 255.0f) * 255.0f;
        result.Data[i + 2] = (Data[i + 2] / 255.0f) * (other.Data[i + 2] / 255.0f) * 255.0f;
    }
    return result;
}

Image Image::difference(const Image& other) const {
   Image result(0, 0);
  
   return result;
}

Image Image::lightest(const Image& other) const {
    if (width() * height() * channels_num() != other.width() * other.height() * other.channels_num()) {
        std::cout << "images must be the same size to call lightest on them" << std::endl;
        return *this;
    }
    Image result(width(), height());
    for (int i = 0; i < width() * height() * channels_num(); ++i) {
        result.Data[i] = std::max(Data[i], other.Data[i]);
    }
    return result;
}
Image Image::darkest(const Image& other) const {
    if (width() * height() * channels_num() != other.width() * other.height() * other.channels_num()) {
        std::cout << "images must be the same size to call darkest on them" << std::endl;
        return *this;
    }
    Image result(width(), height());
    for (int i = 0; i < width() * height() * channels_num(); ++i) {
        result.Data[i] = std::min(Data[i], other.Data[i]);
    }
    return result;
}

Image Image::gammaCorrect(float gamma) const {
    Image result(width(), height());
    for (int i = 0; i < width() * height() * channels_num(); i+=3) {
        result.Data[i] = pow((Data[i] / 255.0f), (1.0f / gamma)) * 255.0f;
        result.Data[i + 1] = pow((Data[i + 1] / 255.0f), (1.0f / gamma)) * 255.0f;
        result.Data[i + 2] = pow((Data[i + 2] / 255.0f), (1 / gamma)) * 255.0f;
    }
    return result;
}

Image Image::alphaBlend(const Image& other, float alpha) const {
    if (width() != other.width() || height() != other.height()) {
        std::cout << "images must have the same dimensions to alpha blend" << std::endl;
        return Image();
    }
    Image result = Image(width(), height());
    for (int i = 0; i < width() * height() * channels_num(); ++i) {
        result.Data[i] = (Data[i] * (1 - alpha)) + (other.Data[i] * alpha);
    }
    return result;
}

Image Image::invert() const {
    Image result(width(), height());
    for (int i = 0; i < width() * height() * channels_num(); ++i) {
        result.Data[i] = 255.0f - Data[i];
    }
    return result;
}

Image Image::grayscale() const {
    Image result(width(), height());
    for (int i = 0; i < height(); ++i) {
        for (int j = 0; j < width(); ++j) {
            unsigned char intensity = Data[(i * width() + j) * channels_num()] / 3.0 + 0.59 * Data[(i * width() + j) * channels_num() + 1] + 0.11 * Data[(i * width() + j) * channels_num() + 2];
            result.Data[(i * width() + j) * channels_num()] = intensity;
            result.Data[(i * width() + j) * channels_num() + 1] = intensity;
            result.Data[(i * width() + j) * channels_num() + 2] = intensity;
        }
    }
    return result;
}

Image Image::distort() const {
    Image result = Image(width(), height());
    for (int i = 0; i < height(); ++i) {
        for (int j = 0; j < width(); ++j) {
            float theta = atan((i - (height() / 2.0f)) / fabs(j - (width() / 2.0f)));
            float r = sqrt(pow(i - height() / 2.0f, 2) + pow(j - width() / 2.0f, 2));
            float px = r * cos(theta + M_PI / 4);
            float py = r * sin(theta + M_PI / 4);
            result.Data[((int)(py * width() + px)) * channels_num()] = Data[i * width() + j];
            result.Data[((int)(py * width() + px)) * channels_num() + 1] = Data[i * width() + j];
            result.Data[((int)(py * width() + px)) * channels_num() + 2] = Data[i * width() + j];
        }
    }
    return result;
}

Image Image::extract(char comp) {
    int offset;
    if (comp == 'r') {
        offset = 0;
    }
    else if (comp == 'g') {
        offset = 1;
    }
    else if (comp == 'b') {
        offset = 2;
    }
    else {
        std::cout << "input parameter must be r, g, or b" << std::endl;
        return *this;
    }
    Image result = *this;
    for (int i = 0; i < width() * height() * channels_num(); i+=3) {
        result.Data[i + offset] = 0;
    }
    return result;
}

Image Image::colorJitter(int size) const {
   Image image(0, 0);
  
   return image;
}

Image Image::bitmap(int size) const {
   Image image(0, 0);
   
   return image;
}

void Image::fill(const Pixel& c) {
  }

}  // namespace agl
