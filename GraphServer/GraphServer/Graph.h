#pragma once
#include <vector>
#include <filesystem>
#include <fstream>

using namespace std; 

const int BYTES_PER_PIXEL = 3;  /// red, green, & blue
const int FILE_HEADER_SIZE = 14;
const int INFO_HEADER_SIZE = 40;

static bool IsPathExist(const std::string& s) {
  struct stat buffer;
  return (stat(s.c_str(), &buffer) == 0);
}

// взято из: https://stackoverflow.com/a/47785639
class Graph {
 public:
  explicit Graph(string path, string filename, vector<double> coeffs, double from, double to) {
      /// red, green, & blue
    const int height = 100;
    const int width = 100;
    unsigned char image[height][width][BYTES_PER_PIXEL];

    
    auto f = [=](auto x) { 
        auto sum = coeffs[0];
        auto degree = x;
        for (int i = 1; i < coeffs.size(); i++) {
          sum += degree * coeffs[i];
          degree *= x;
        }  
        return sum;      
    };
    
        
    int i, j;

    for (i = 0; i < height; i++) {
      for (j = 0; j < width; j++) {
        image[i][j][2] = (unsigned char)(255);  /// red
        image[i][j][1] = (unsigned char)(255);  /// green
        image[i][j][0] = (unsigned char)(255);  /// blue
      }
    }

    int center_x = width / 2;
    int center_y = height / 2;

    // vertical line
    for (i = 0; i < height; i++) {
      image[i][center_x][2] = (unsigned char)(0);  /// red
      image[i][center_x][1] = (unsigned char)(0);  /// green
      image[i][center_x][0] = (unsigned char)(0);  /// blue
    }

    // horizontal line
    for (i = 0; i < width; i++) {
      image[center_y][i][2] = (unsigned char)(0);  /// red
      image[center_y][i][1] = (unsigned char)(0);  /// green
      image[center_y][i][0] = (unsigned char)(0);  /// blue
    }

    int segment_start = from;  //-width / 2;
    int segment_end = to;   // width / 2;
    int old_y = f(segment_start);

    for (i = min(segment_start, -width/2); i < max(segment_end, width/2); i++) {
      int new_y = f(i) + center_y;

      // interpolation
      int x_coord = min(i + center_x, width - 1);
      for (int k = old_y; k <= new_y; k++) {
        if (k >= 0 && k < height) {
          image[k][x_coord][2] = (unsigned char)(0);  /// red
          image[k][x_coord][1] = (unsigned char)(0);  /// green
          image[k][x_coord][0] = (unsigned char)(0);  /// blue
        }
      }

      // interpolation
      for (int k = new_y; k <= old_y; k++) {
        if (k >= 0 && k < height) {
          image[k][x_coord][2] = (unsigned char)(0);  /// red
          image[k][x_coord][1] = (unsigned char)(0);  /// green
          image[k][x_coord][0] = (unsigned char)(0);  /// blue
        }
      }
      old_y = new_y;
    }

    generateBitmapImage((unsigned char*)image, height, width,
                        filename, path);
  }

  vector<char> get_data_binary() {
    std::ifstream file(full_path, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {      
      return buffer;
    }    
  }

  static vector<char> get_data_binary(string dir, string filename) {
    string full_path = dir + "\\" + filename + ".bmp\0";

    std::ifstream file(full_path, std::ios::binary | std::ios::ate);

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {      
      return buffer;
    }
  }

  private:
 void generateBitmapImage(unsigned char* image, int height, int width,
                           string filename, string path) {
    int widthInBytes = width * BYTES_PER_PIXEL;

    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4 - (widthInBytes) % 4) % 4;

    int stride = (widthInBytes) + paddingSize;
 

    if (!IsPathExist(path)) {
      string mkdir = "mkdir " + path;
      system(mkdir.c_str());
    }    
    full_path = path + "\\" + filename + ".bmp\0";
    FILE* imageFile = fopen(full_path.c_str(), "wb");
    
    

    unsigned char* fileHeader = createBitmapFileHeader(height, stride);
    fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);

    unsigned char* infoHeader = createBitmapInfoHeader(height, width);
    fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

    int i;
    for (i = 0; i < height; i++) {
      fwrite(image + (i * widthInBytes), BYTES_PER_PIXEL, width, imageFile);
      fwrite(padding, 1, paddingSize, imageFile);
    }

    fclose(imageFile);
  }

  unsigned char* createBitmapFileHeader(int height, int stride) {
    int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

    static unsigned char fileHeader[] = {
        0, 0,        /// signature
        0, 0, 0, 0,  /// image file size in bytes
        0, 0, 0, 0,  /// reserved
        0, 0, 0, 0,  /// start of pixel array
    };

    fileHeader[0] = (unsigned char)('B');
    fileHeader[1] = (unsigned char)('M');
    fileHeader[2] = (unsigned char)(fileSize);
    fileHeader[3] = (unsigned char)(fileSize >> 8);
    fileHeader[4] = (unsigned char)(fileSize >> 16);
    fileHeader[5] = (unsigned char)(fileSize >> 24);
    fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

    return fileHeader;
  }

  unsigned char* createBitmapInfoHeader(int height, int width) {
    static unsigned char infoHeader[] = {
        0, 0, 0, 0,  /// header size
        0, 0, 0, 0,  /// image width
        0, 0, 0, 0,  /// image height
        0, 0,        /// number of color planes
        0, 0,        /// bits per pixel
        0, 0, 0, 0,  /// compression
        0, 0, 0, 0,  /// image size
        0, 0, 0, 0,  /// horizontal resolution
        0, 0, 0, 0,  /// vertical resolution
        0, 0, 0, 0,  /// colors in color table
        0, 0, 0, 0,  /// important color count
    };

    infoHeader[0] = (unsigned char)(INFO_HEADER_SIZE);
    infoHeader[4] = (unsigned char)(width);
    infoHeader[5] = (unsigned char)(width >> 8);
    infoHeader[6] = (unsigned char)(width >> 16);
    infoHeader[7] = (unsigned char)(width >> 24);
    infoHeader[8] = (unsigned char)(height);
    infoHeader[9] = (unsigned char)(height >> 8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL * 8);

    return infoHeader;
  }
   
  string full_path;
};
