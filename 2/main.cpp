#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <dirent.h>
#include <fstream>


using namespace std;
#define READ 0
#define WRITE 1
#define OUT_SIZE 1000000
#define OUT_FILE "output.csv"

const char * named_pipe="mypipe";

int count_files(char* path){
    DIR *dp;
    int i = 0;
    struct dirent *ep;     
    dp = opendir(path);

    if (dp != NULL)
    {
        while (ep = readdir (dp))
        i++;

        (void) closedir (dp);
    }
    else
        perror ("Couldn't open the directory");

        return i-2;
}
char *stcp(string str){
    char * writable = new char[str.size() + 1];
    copy(str.begin(), str.end(), writable);
    writable[str.size()] = '\0'; 
    return writable;
}
void mapper(int count){
    for (int i = 1; i <=count; i++){
        int Pipe_to_map[2];
		pipe(Pipe_to_map);//make pipe
        pid_t pid = fork();//make child process
        if (pid == 0){//child process
            sleep(i-1);
            execlp("./map.out",stcp(to_string(Pipe_to_map[READ])),stcp(to_string(Pipe_to_map[WRITE])),named_pipe,NULL);
        }
        else{//parent process
            string add="testcases/"+to_string(i) + ".csv";
            close(Pipe_to_map[READ]);//write to the first of the pipe
            write(Pipe_to_map[WRITE],add.c_str(),add.length()+1);
            close(Pipe_to_map[WRITE]);
        }
    }
}
void reducer(int count){
    int Pipe_to_reduce[2];
    pipe(Pipe_to_reduce);
    pid_t pid =fork();
    if(pid==0)
        execlp("./reduce.out",stcp(to_string(Pipe_to_reduce[READ])),stcp(to_string(Pipe_to_reduce[WRITE])),named_pipe,stcp(to_string(count)),NULL);
    else{
        close(Pipe_to_reduce[WRITE]);
        char result[OUT_SIZE];
        read(Pipe_to_reduce[READ],result,OUT_SIZE);
        close(Pipe_to_reduce[READ]);
        ofstream result_file;
        result_file.open(OUT_FILE);
        result_file << result;
        result_file.close();

    }

}
int main(){
    int count=count_files("testcases"); //count files in directory
    mapper(count);//make mapper processes
    //reduce process
    reducer(count);
return 0;
}
