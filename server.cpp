#include <iostream>
#include <chrono>
#include <thread>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "request.hpp"
#include "response.hpp"

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

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

    uint16_t port = 8080;

    struct sockaddr_in socket_addr;

    socket_addr.sin_family      = AF_INET;
    socket_addr.sin_addr.s_addr = INADDR_ANY;
    socket_addr.sin_port        = htons(port);

    uint8_t TIMEOUT_LIMIT = 5;

    for(int i = 0; i < TIMEOUT_LIMIT; i++){
        if(bind(server_socket, (struct sockaddr*)&socket_addr, sizeof(socket_addr)) > -1) 
            break;
        if(i < TIMEOUT_LIMIT - 1){
            cout << "Error binding socket. Retrying..." << endl;
            sleep_until(system_clock::now() + seconds(1));
        }
        else{
            cout << '\n' << "Socket not able to be bound. Terminating..." << endl;
            exit(EXIT_FAILURE);
        }   
    }

    cout << "Socket bound to port " << port << endl;

    uint16_t UDP_MAX_SIZE = 512;
    uint16_t TCP_MAX_SIZE = 64000;

    char buffer[UDP_MAX_SIZE];
    int buffer_len = UDP_MAX_SIZE;

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof (client_addr);

    init_zonedata("data/zonedata");

    while(true){
        int bytes = recvfrom(server_socket, buffer, buffer_len, 0, (struct sockaddr*)&client_addr, &addr_len);
        
        Request request;
        request.decode_request(buffer);
        
        sendto(server_socket, buffer, buffer_len, 0, (struct sockaddr*)&client_addr, addr_len);
    }
}
