#include <fstream>
#include <iostream>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <unistd.h>
#include <vector>
#include <map>

#define READ 0
#define WRITE 1
#define SIZE 1000
#define OUT_SIZE 1000000

using namespace std;
map<string ,int> make_map(string in){
    map<string ,int> out;
    size_t start;
    size_t end = 0;
    while ((start = in.find_first_not_of(',', end)) != string::npos){
        end = in.find(',', start);
        string str = in.substr(start, end - start);
        size_t delim = str.find(':');
        string key = str.substr(0, delim);
        int value;
        stringstream temp(str.substr(delim+1, end-start-delim-1));
        temp >> value;
        if(out.find(key) == out.end()){
            out.insert(pair<string ,int>(key, value));
        }
        else{

            int curr_value = out.find(key)->second;
            out.find(key)->second = curr_value+value;
        }
    }
    return out;
}
void convert_to_string(map<string, int> in, char* out){
    map<string, int>::iterator itr;
    for (itr = in.begin(); itr != in.end(); itr++) {
        strcat(out, itr->first.c_str());
        strcat(out, ":");
        strcat(out, to_string(itr->second).c_str());
        strcat(out, ",");
    }
}
int main(int argc, char* argv[]){
    close(atoi(argv[READ]));
    char out[OUT_SIZE];
    int num_of_maps = atoi(argv[3]);
    mkfifo(argv[2], 0666);
    sleep(0.5);
    int data = open(argv[2], O_RDONLY);
    for(int i = 0; i < num_of_maps; i++){
        char temp[OUT_SIZE];
        while(read(data, temp, OUT_SIZE)<=0);
        strcat(out, temp);
    }
    close(data);
    map<string ,int> merge_map = make_map(out);
    char output[OUT_SIZE] = "";
    convert_to_string(merge_map, output);
    write(atoi(argv[WRITE]), output, strlen(output)+1);
    close(atoi(argv[WRITE]));
    
    return 0;
}