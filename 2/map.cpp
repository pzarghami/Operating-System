#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <unistd.h>
#include <bits/stdc++.h>
#include <fcntl.h>
#include <sys/stat.h>



using namespace std;

map<string,int> words;

#define READ 0
#define WRITE 1
#define SIZE 1000
#define OUT_SIZE 100000

vector<string> split(string words){
    string word="";
    vector<string> my_words;
    for(int i=0; i<=words.length();i++){
        if(words[i]==','){
            my_words.push_back(word);
            word.clear();
            continue;
        }
        else if(i==words.length()){
            my_words.push_back(word);
            word.clear(); 
            continue;
        }
        else word.push_back(words[i]);
    }
    return my_words;
}
void mapping(vector<string> split_words){
    for(int i=0;i<split_words.size();i++){
        if(split_words[i]=="")
            continue;
        if(words.find(split_words[i])==words.end())
            words[split_words[i]]=1;
        else
            words[split_words[i]]++;
    }
}
void read_file(string address_file){
    ifstream readfile;
    readfile.open(address_file);
    string words;
    getline(readfile,words);
    vector<string> split_words=split(words);
    mapping(split_words);
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
    int read_pipe = atoi(argv[READ]);
    int write_pipe = atoi(argv[WRITE]);
    char address_file[SIZE];
    close(write_pipe);//close end of the pipe
    read(read_pipe,address_file,SIZE+1);//read address from pipe
    close(read_pipe);//close the first of th pipe
    read_file(address_file);//read frome file and map it
    char out_str[OUT_SIZE];
    convert_to_string(words, out_str);
    mkfifo(argv[2],0666);//make namedpiped
    int fd=open(argv[2],O_WRONLY);
    write(fd,out_str,OUT_SIZE);
    close(fd);
    


}