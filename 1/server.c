#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#define ROOM_SIZE 10
#define Local_Port 8082


typedef struct Room{
    int Clients[3];
    int num_cl;
    int port;
    int name;
}Room;
Room* Computer_rooms;
Room* Mechanic_rooms;
Room* Civil_rooms;
Room* Electrical_rooms;
int new_port=Local_Port+1;
int setupServer(int port) {
    struct sockaddr_in address;
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    
    listen(server_fd, 4);

    return server_fd;
}

int acceptClient(int server_fd) {
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*) &address_len);

    return client_fd;
}

void send_message(int id,char str[1024]){
    send(id,str,strlen(str),0);
}
int dublicate_client(int id, Room* myroom){
    for(int i=0;i<10;i++){
        for(int j=0;j<3;j++){
            if(myroom[i].Clients[j]==id)
                return 1;
        }
    }
    return 0;
}
void send_to_all(char* buff,Room* new_room){
    for(int i=0;i<3;i++){
        send(new_room->Clients[i],buff,strlen(buff),0);
    }
}
void make_room(Room* new_room){
    char buff[1024];
    new_room->port=new_port;
    sprintf(buff,"%d %d %d %d %d is your id room",new_port,new_room->Clients[0],new_room->Clients[1],new_room->Clients[2],new_room->name);
    new_port++;
    send_to_all(buff,new_room);
}
void clear_room(Room* new_room){
    new_room->port=0;
    new_room->num_cl=0;
    for(int i=0;i<3;i++){
        new_room->Clients[i]=0;
    }
}

void wich_room(int id,char* buff,char* str,Room* myroom){
    if(dublicate_client(id,myroom))
        return;
    if(!strcmp(buff,str)){
        for(int i=0;i<10;i++){
            if(myroom[i].num_cl<3){
                myroom[i].Clients[myroom[i].num_cl]=id;
                myroom[i].num_cl++;
                if(myroom[i].num_cl==3){
                make_room(&myroom[i]);
                clear_room(&myroom[i]);
                return;
                }
                send_message(id,"Please wait. Room capacity is not enough!\n");
                break;
            }
        }
    }
}
void select_type(int id,char* buff,int bytes_received){
    char temp[bytes_received];
    strcpy(temp,buff);
    wich_room(id,temp,"Computer\n",Computer_rooms);
    wich_room(id,temp,"Civil\n",Civil_rooms);
    wich_room(id,temp,"Electrical\n",Electrical_rooms);
    wich_room(id,temp,"Mechanic\n",Mechanic_rooms);
}
void initial_Rooms(){
    for(int i=0;i<10;i++){
        Computer_rooms[i].num_cl=0;
        Computer_rooms[i].name=1;
    }
    for(int i=0;i<10;i++){
        Mechanic_rooms[i].num_cl=0;
        Mechanic_rooms[i].name=2;
    }
    for(int i=0;i<10;i++){
        Electrical_rooms[i].num_cl=0;
        Electrical_rooms[i].name=3;
    }
    for(int i=0;i<10;i++){
        Civil_rooms[i].num_cl=0;
        Civil_rooms[i].name=4;
    }
}
void save_chat_room(char buff[1024],int id_room){
    int file;
    char name[30];
    if (id_room == 1)
    {
        strcpy(name, "Computer.txt");
    }
    else if (id_room == 2)
    {
        strcpy(name, "Mechanic.txt");
    }
    else if (id_room == 3)
    {
        strcpy(name, "Electrical.txt");
    }
    else
        strcpy(name, "Civil.txt");
    file = open(name, O_APPEND | O_RDWR);
    write(file, buff, strlen(buff));
    close(file);
}
int main(int argc, char const *argv[]) {
    int server_fd, new_socket, max_sd;
    Computer_rooms=(Room*)malloc(sizeof(Room)*ROOM_SIZE);
    Mechanic_rooms=(Room*)malloc(sizeof(Room)*ROOM_SIZE);
    Civil_rooms=(Room*)malloc(sizeof(Room)*ROOM_SIZE);
    Electrical_rooms=(Room*)malloc(sizeof(Room)*ROOM_SIZE);
    char buffer[1024] = {0};
    fd_set master_set, working_set;
    initial_Rooms();
    server_fd = setupServer(8080);

    FD_ZERO(&master_set);
    max_sd = server_fd;
    FD_SET(server_fd, &master_set);

    write(1, "Server is running\n", 18);
    char buff[1024]={0};
    while (1) {
        working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);

        for (int i = 0; i <= max_sd; i++) {
            if (FD_ISSET(i, &working_set)) {
                
                if (i == server_fd) {  // new clinet
                    new_socket = acceptClient(server_fd);
                    sprintf(buff,"%d is your id\n",new_socket);
                    send(new_socket,buff,1024,0);
                    memset(buffer, 0, 1024);
                    FD_SET(new_socket, &master_set);
                    if (new_socket > max_sd)
                        max_sd = new_socket;
                    printf("New client connected. fd = %d\n", new_socket);
                    send_message(new_socket,"Please Enter your Rooms name you want:\n-Computer\n-Civil\n-Electrical\n-Mechanic\n");
                }
                
                else { // client sending msg
                    int bytes_received;
                    bytes_received = recv(i , buffer, 1024, 0);
                    if (bytes_received == 0) { // EOF
                        printf("client fd = %d closed\n", i);
                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }
                    if(buffer[0]=='@'){
                        int id_room=atoi(&buffer[1]);
                        save_chat_room(buffer,id_room);
                    }
                    else{
                    select_type(i,buffer,bytes_received-1);
                    printf("client %d: %s\n", i, buffer);
                    memset(buffer, 0, 1024);
                    }
                }
            }
        }

    }

    return 0;
}
