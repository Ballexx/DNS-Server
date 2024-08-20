#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

int create_socket(){
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if(server_socket < 0){

        cout << "Socket creation failed. Terminating..." << endl;
        exit(EXIT_FAILURE);   
    }

    return server_socket;
}

int main(){
    int server_socket = create_socket();

    cout << "Socket created" << endl;

    int port = 3000;

    struct sockaddr_in socket_addr;
    socket_addr.sin_family      = AF_INET;
    socket_addr.sin_addr.s_addr = INADDR_ANY;
    socket_addr.sin_port        = htons(port);

    for(int i = 0; i < 5; i++){
        if(bind(server_socket, (struct sockaddr*)&socket_addr, sizeof(socket_addr)) > -1) 
            break;

        if(i < 5){
            cout << "Error binding socket. Retrying..." << endl;
        }
        else{
            cout << "Socket not able to be bound. Terminating..." << endl;
            exit(EXIT_FAILURE);
        }   
    }

    cout << "Socket bound to port " << port << endl;

    int UDP_MAX_SIZE = 512;
    int TCP_MAX_SIZE = 64000;

    char buffer[UDP_MAX_SIZE];
    int buffer_len = UDP_MAX_SIZE;

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof (client_addr);

    while(true){
        int bytes = recvfrom(server_socket, buffer, buffer_len, 0, (struct sockaddr*)&client_addr, &addr_len);

        if(bytes > 0 ){
            buffer[bytes] = 0;
        }

        for(int i = 0; i < buffer_len; i++){
            cout << buffer[i];
        }

        sendto(server_socket, buffer, bytes, 0, (struct sockaddr*)&client_addr, addr_len);
    }

}