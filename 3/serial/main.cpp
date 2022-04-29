#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstring>
#include <bits/stdc++.h>
#include <string>

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;

using namespace std;

#pragma pack(1)
#pragma once

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct tagBITMAPFILEHEADER
{
  WORD bfType;
  DWORD bfSize;
  WORD bfReserved1;
  WORD bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
  DWORD biSize;
  LONG biWidth;
  LONG biHeight;
  WORD biPlanes;
  WORD biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG biXPelsPerMeter;
  LONG biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

int rows;
int cols;
vector<vector<vector<int>>> image;
bool fillAndAllocate(char *&buffer, const char *fileName, int &rows, int &cols, int &bufferSize)
{
  std::ifstream file(fileName);

  if (file)
  {
    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = new char[length];
    file.read(&buffer[0], length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;

    file_header = (PBITMAPFILEHEADER)(&buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    bufferSize = file_header->bfSize;
    return 1;
  }
  else
  {
    cout << "File" << fileName << " doesn't exist!" << endl;
    return 0;
  }
}

void getPixlesFromBMP24(int end, int rows, int cols, char *fileReadBuffer, vector<vector<vector<int>>> &image)
{
  int count = 1;
  int extra = cols % 4;
  for (int i = 0; i < rows; i++)
  {
    count += extra;
    for (int j = cols - 1; j >= 0; j--)
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          image[i][j][0] = (unsigned char)fileReadBuffer[end - count];
          break;
        case 1:
          image[i][j][1] = (unsigned char)fileReadBuffer[end - count];
          break;
        case 2:
          image[i][j][2] = (unsigned char)fileReadBuffer[end - count];
          break;
        }
        // go to the next position in the buffer
        count++;
      }
  }
}

void writeOutBmp24(char *fileBuffer, const char *nameOfFileToCreate, int bufferSize, vector<vector<vector<int>>> &image)
{
  std::ofstream write(nameOfFileToCreate);
  if (!write)
  {
    cout << "Failed to write " << nameOfFileToCreate << endl;
    return;
  }
  int count = 1;
  int extra = cols % 4;
  for (int i = 0; i < rows; i++)
  {
    count += extra;
    for (int j = cols - 1; j >= 0; j--)
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          fileBuffer[bufferSize - count] = (unsigned char)image[i][j][0];
          break;
        case 1:
          fileBuffer[bufferSize - count] = (unsigned char)image[i][j][1];
          break;
        case 2:
          fileBuffer[bufferSize - count] = (unsigned char)image[i][j][2];
          break;
        }
        // go to the next position in the buffer
        count++;
      }
  }
  write.write(fileBuffer, bufferSize);
}
double get_taken_time(clock_t t1,clock_t t2){
    return double(t2-t1)/double(CLOCKS_PER_SEC);
}
int mean_box(int start_i,int end_i,int start_j,int end_j,int k,vector<vector<vector<int>>> &image){
    int sum=0;
    //cout<<start_i-end_i<<"--"<<start_j-end_j<<endl;
    for(int i=start_i;i<=end_i;i++){
        for(int j=start_j;j<=end_j;j++)
            sum+=image[i][j][k];
    }
    return sum/9;
        
}
void Smoothing(vector<vector<vector<int>>> &image){
    for(int i=1;i<rows-1;i++)
        for(int j=1;j<cols-1;j++){
            for(int k=0;k<3;k++)
                image[i][j][k]=mean_box(i-1,i+1,j-1,j+1,k,image);
        }

}
void Sepia(vector<vector<vector<int>>> &image){
    for(int i=0;i<rows;i++)
        for(int j=0;j<cols;j++)
            for(int k=0;k<3;k++){
                switch (k){
                    case 0:
                    image[i][j][k] = (image[i][j][0] * 0.393) + (image[i][j][1] * 0.769) + (image[i][j][2] * 0.189);
                    break;
                    case 1:
                    image[i][j][k]= (image[i][j][0] * 0.349) + (image[i][j][1] * 0.686) + (image[i][j][2] * 0.168);
                    break;
                    case 2:
                     image[i][j][k]=(image[i][j][0] * 0.272) + (image[i][j][1] * 0.534) + (image[i][j][2] * 0.131);
                    break;
                    }
                if(image[i][j][k] > 255)
                    image[i][j][k] = 255;
            }
}
void washed_out(vector<vector<vector<int>>> &image){
    int sum_red=0,sum_green=0,sum_blue=0;
    for(int i=0;i<rows;i++)
      for(int j=0;j<cols;j++){
        sum_red+=image[i][j][0];
        sum_green+=image[i][j][1];
        sum_blue+=image[i][j][2];
      }
    int num=rows*cols;
    for(int i=0;i<rows;i++)
      for(int j=0;j<cols;j++){
        image[i][j][0]= image[i][j][0] * 0.4 + (sum_red/num)* 0.6;
        image[i][j][1]= image[i][j][1] * 0.4 + (sum_green/num)* 0.6;
        image[i][j][2]= image[i][j][2] * 0.4 + (sum_blue/num)* 0.6;
      }
}
void add_lines(vector<vector<vector<int>>> &image){
  for(int i = 0; i < rows; i++)
    for(int j = 0; j < cols; j++){
      if(i == j || (i-1) == j || (i+1) == j || i == (cols-j) || (i-1) == (cols-j) || (i+1) == (cols-j))
        image[i][j][0] = image[i][j][1] = image[i][j][2] = 255;
    }
}
int main(int argc, char *argv[]){
    char *fileBuffer;
    int bufferSize;
    char *fileName = argv[1];
    clock_t start , end,all_start,all_end;
    all_start=clock();
    if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize)){
        cout << "File read error" << endl;
        return 1;
    }
    vector<int> a(3, 0);
    vector<vector<int>> temp(rows, a);
    vector<vector<vector<int>>> image(cols, temp);
    start=clock();
    getPixlesFromBMP24(bufferSize,rows,cols,fileBuffer,image);
    end=clock();
    //cout<<double(t2-t1)/double(CLOCKS_PER_SEC)<<setprecision(5);
    double read_file_tm=get_taken_time(start,end);
    start=clock();
    Smoothing(image);
    end=clock();
    double smoothing_tm=get_taken_time(start,end);
    start=clock();
    Sepia(image);
    end=clock();
    double sepia_tm=get_taken_time(start,end);
    start=clock();
    washed_out(image);
    end=clock();
    double washed_out_tm=get_taken_time(start,end);
    start=clock();
    add_lines(image);
    end=clock();
    double lines_tm=get_taken_time(start,end);
    string output_address="output.bmp";
    int n = output_address.length() + 1;
    char out_file[n];
    strcpy(out_file, output_address.c_str());
    start=clock();
    writeOutBmp24(fileBuffer, out_file, bufferSize, image);
    end=clock();
    double write_file_tm=get_taken_time(start,end);
    all_end=clock();
    //for finding hotspots:
    cout << "Time of Reading from file (ms): " <<read_file_tm<< setprecision(5)<<endl;
    cout << "Time of Smoothing Filter (ms): " << smoothing_tm<<setprecision(5)<< endl;
    cout << "Time of Sepia Filter (ms): " <<sepia_tm<<setprecision(5)<< endl;
    cout << "Time of Washed Out Filter (ms): " << washed_out_tm<<setprecision(5) << endl;
    cout << "Time of Add Line Filter (ms): " << lines_tm<<setprecision(5) << endl;
    cout << "Time of Writing in file (ms): " << write_file_tm<<setprecision(5) << endl;
    cout << "Serial Execution Time (ms): " << get_taken_time(all_start,all_end)<<setprecision(5) << endl;


    // apply filters
  // write output file

    return 0;
}