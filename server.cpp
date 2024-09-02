#include <iostream>
#include <chrono>
#include <thread>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

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

static const uint32_t QR_MASK       = 0x8000;
static const uint32_t OPCODE_MASK   = 0x7800;
static const uint32_t AA_MASK       = 0x0400;
static const uint32_t TC_MASK       = 0x0200;
static const uint32_t RD_MASK       = 0x0100;
static const uint32_t RA_MASK       = 0x8000;
static const uint32_t RCODE_MASK    = 0x000F;

class Response{
    private:
    uint16_t buffer_index = 0;

    uint32_t read_16bit(char* buffer){
        uint32_t bit_value = static_cast<unsigned char>(buffer[buffer_index]);

        bit_value = bit_value << 8;
        bit_value += static_cast<unsigned char>(buffer[buffer_index + 1]);

        buffer_index += 2;
        return bit_value;
    }

    uint32_t read_query(char* buffer){
        uint8_t header_len = 12;
        uint8_t query_len = 13;

        for(int i = header_len; i < header_len + query_len; i++){
            cout << (buffer[i] << 8) << endl;
        }
        return 1;
    }

    void put_16bit(char* buffer, uint32_t value){

    }

    uint32_t TRANSACTION_ID;
    uint32_t BIT_FIELDS; 

    uint32_t QR;
    uint32_t OPCODE;
    uint32_t AA;
    uint32_t TC;
    uint32_t RD;
    uint32_t RA;
    uint32_t Z;
    uint32_t RCODE;

    uint32_t QD_COUNT;
    uint32_t AN_COUNT;
    uint32_t NS_COUNT;
    uint32_t AR_COUNT;

    uint32_t QUERY;

    public:
    void decode_request(char* buffer){
        TRANSACTION_ID = read_16bit(&buffer[buffer_index]);
        BIT_FIELDS =     read_16bit(&buffer[buffer_index]);

        QR =      (BIT_FIELDS & QR_MASK);
        OPCODE =  (BIT_FIELDS & OPCODE_MASK);
        AA =      (BIT_FIELDS & AA_MASK);
        TC =      (BIT_FIELDS & TC_MASK);
        RD =      (BIT_FIELDS & RD_MASK);
        RA =      (BIT_FIELDS & RA_MASK);
        RCODE =   (BIT_FIELDS & RCODE_MASK);

        QD_COUNT = read_16bit(&buffer[buffer_index]);
        AN_COUNT = read_16bit(&buffer[buffer_index]);
        NS_COUNT = read_16bit(&buffer[buffer_index]);
        AR_COUNT = read_16bit(&buffer[buffer_index]);

        for(int i = 0; i < 13; i++){
            cout << buffer[buffer_index+i];
        }
    }
};

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

    while(true){
        int bytes = recvfrom(server_socket, buffer, buffer_len, 0, (struct sockaddr*)&client_addr, &addr_len);

        Response response;
        response.decode_request(buffer);

        //memset(buffer, 0, buffer_len);
        
        sendto(server_socket, buffer, buffer_len, 0, (struct sockaddr*)&client_addr, addr_len);
    }

}