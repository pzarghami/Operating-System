#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstring>
#include <bits/stdc++.h>
#include <string>
#include <pthread.h>

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

#define NUM_THREADS 2
#define NUM_SMOTH 3
#define NUM_THREADS_READ 5
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
struct thread_data {
   int start;
   int end;
};
struct getPixsArgs
{
  int count, row, maxR, extra, end, id;
  char *fileReadBuffer;
};

int rows;
int cols;
vector<vector<vector<int>>>  image;
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
char* temp;
void* write_thread(void* threadarg){
  struct getPixsArgs *args;
  args = (struct getPixsArgs *)threadarg;
  int count = args->count;
  for (int i = args->row; i < args->maxR; i++){
    count += args->extra;
    for (int j = cols - 1; j >= 0; j--){
      temp[args->end - count] = (unsigned char)image[i][j][0];
      count++;
      temp[args->end - count] = (unsigned char)image[i][j][1];
      count++;
      temp[args->end - count] = (unsigned char)image[i][j][2];
      count++;
    }
  }
  pthread_exit(NULL);
}
void write_file_threading(int end, char *buf = new char[0]){
  pthread_t threads[NUM_THREADS_READ];
  struct getPixsArgs td[NUM_THREADS_READ + 1];
  int rc;
  int parts = rows / NUM_THREADS_READ;
  int start = 0;
  int extra = cols % 4;
  int i;
  for (i = 0; i < NUM_THREADS_READ; i++){
    td[i].end = end;
    td[i].extra = extra;
    td[i].row = start;
    td[i].maxR = start + parts;
    td[i].count = cols * i * parts * 3 + 1;
    td[i].fileReadBuffer = buf;
    start += parts;
    rc = pthread_create(&threads[i], NULL, write_thread, (void *)&td[i]);
  }
  if (rows % NUM_THREADS_READ != 0)
  {
    pthread_t thread;

    td[i].end = end;
    td[i].extra = extra;
    td[i].row = start;
    td[i].maxR = start + (rows % NUM_THREADS_READ);
    start += parts;
    td[i].count = cols * i * parts * 3 + 1;
    td[i].fileReadBuffer = buf;
    rc = pthread_create(&thread, NULL, write_thread, (void *)&td[i]);
    pthread_join(thread, NULL);
  }
  for (int i = 0; i < NUM_THREADS_READ; ++i)
  {
    pthread_join(threads[i], NULL);
  }
}

void *read_thread(void *threadarg){
  struct getPixsArgs *args;
  args = (struct getPixsArgs *)threadarg;
  int count = args->count;

  for (int i = args->row; i < args->maxR; i++){
    count += args->extra;
    for (int j = cols - 1; j >= 0; j--){
      for (int k = 0; k < 3; k++){
        image[i][j][k] = (unsigned char)args->fileReadBuffer[args->end - count];
        count++;
      }
    }
  }
  pthread_exit(NULL);
}
void read_file_threading(int end, char *buf = new char[0]){
  pthread_t threads[NUM_THREADS_READ];
  struct getPixsArgs td[NUM_THREADS_READ + 1];
  int rc;
  int parts = rows / NUM_THREADS_READ;
  int start = 0;
  int extra = cols % 4;
  int i;
  for (i = 0; i < NUM_THREADS_READ; i++){
    td[i].end = end;
    td[i].extra = extra;
    td[i].row = start;
    td[i].maxR = start + parts;
    td[i].count = cols * i * parts * 3 + 1;
    td[i].fileReadBuffer = buf;
    start += parts;
    rc = pthread_create(&threads[i], NULL, read_thread, (void *)&td[i]);
  }
  if (rows % NUM_THREADS_READ != 0)
  {
    pthread_t thread;

    td[i].end = end;
    td[i].extra = extra;
    td[i].row = start;
    td[i].maxR = start + (rows % NUM_THREADS_READ);
    start += parts;
    td[i].count = cols * i * parts * 3 + 1;
    td[i].fileReadBuffer = buf;
    rc = pthread_create(&thread, NULL, read_thread, (void *)&td[i]);
    pthread_join(thread, NULL);
  }
  for (int i = 0; i < NUM_THREADS_READ; ++i)
  {
    pthread_join(threads[i], NULL);
  }
}
void writeOutBmp24(char *fileBuffer, const char *nameOfFileToCreate, int bufferSize)
{
  temp = fileBuffer;
  std::ofstream write(nameOfFileToCreate);
  if (!write){
    cout << "Failed to write " << nameOfFileToCreate << endl;
    return;
  }
  write_file_threading(bufferSize, temp);
  write.write(temp, bufferSize);
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
void *Sepia(void *threadarg) {
   struct thread_data *my_data;
   my_data = (struct thread_data *) threadarg;
   int sum=0;
   for(int i=my_data->start;i<=my_data->end;i++){
        for(int j=0;j<cols;j++){
            image[i][j][0] = (image[i][j][0] * 0.393) + (image[i][j][1] * 0.769) + (image[i][j][2] * 0.189);
            image[i][j][1]= (image[i][j][0] * 0.349) + (image[i][j][1] * 0.686) + (image[i][j][2] * 0.168);
            image[i][j][2]=(image[i][j][0] * 0.272) + (image[i][j][1] * 0.534) + (image[i][j][2] * 0.131);
        for(int k=0;k<3;k++)
          if(image[i][j][k] > 255)
            image[i][j][k] = 255;
        }
   }
   pthread_exit(NULL);
}
void* Smoothing(void* threadarg){
  long color=(long) threadarg;
    for(int i=1;i<rows-1;i++)
        for(int j=1;j<cols-1;j++){
            image[i][j][color]=mean_box(i-1,i+1,j-1,j+1,color,image);
        }
  pthread_exit(NULL);

}
void make_tread_for_smooth(){
  pthread_t threads[NUM_SMOTH];
  int color=0;
  int rc;
  for(int i=0;i<NUM_SMOTH;i++){
    color=0;
    rc=pthread_create(&threads[i],NULL,Smoothing,(void*)color);
    color++;
  }
  for (int i = 0; i < NUM_SMOTH; ++i){
      pthread_join(threads[i], NULL);
  }
}
void make_tread_for_sepia(){
    pthread_t threads[NUM_THREADS];
    struct thread_data  td[NUM_THREADS];
    int Start=0;
    int rc;
    int End=(rows/NUM_THREADS)-1;
    int i;
    for(i=0;i<NUM_THREADS;i++){
        td[i].start=Start;
        td[i].end=End;
        rc=pthread_create(&threads[i],NULL,Sepia,(void*)&td[i]);
        Start+=rows/NUM_THREADS;
        End+=rows/NUM_THREADS;

    }
    for (int i = 0; i < NUM_THREADS; ++i){
      pthread_join(threads[i], NULL);
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
    vector<vector<vector<int>>> mage(cols, temp);
    image=mage;
    read_file_threading(bufferSize,fileBuffer);
    make_tread_for_smooth();
    make_tread_for_sepia();
    washed_out(image);
    add_lines(image);
    string output_address="output.bmp";
    int n = output_address.length() + 1;
    char out_file[n];
    strcpy(out_file, output_address.c_str());
    writeOutBmp24(fileBuffer, out_file, bufferSize);   
    all_end=clock();
    cout<< "Serial Execution Time (ms): " << get_taken_time(all_start,all_end)<<setprecision(5) << endl;
    cout<<0.12388/get_taken_time(all_start,all_end)<<setprecision(5)<<endl;
  // write output file

    return 0;
}