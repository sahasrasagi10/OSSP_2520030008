                                                                     socket_server.c
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>


#define SOCKET_PATH "/tmp/my_socket"

int main(){

int server_fd, client_fd;
struct sockaddr_un address;
char buffer[100];


//create socket
server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

//set  socket address
address.sun_family = AF_UNIX;
strcpy(address.sun_path, SOCKET_PATH);

// remove old socket
unlink(SOCKET_PATH);

//Bind socket to opath
bind(server_fd, (struct sockaddr *)&address, sizeof(address));

//wait for client
listen(server_fd,5);

printf("Server waiting...\n");

//accept client
client_fd = accept(server_fd,NULL,NULL);

//receive message
read(client_fd, buffer, sizeof(buffer));

printf("Message from client: %s\n", buffer);

close(client_fd);
close(server_fd);

unlink(SOCKET_PATH);

return 0;
