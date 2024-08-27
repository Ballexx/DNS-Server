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
    uint32_t convert_to_16bit(const char* buffer){
        uint32_t bit_value = static_cast<unsigned char>(buffer[0]);
        bit_value = bit_value << 8;
        bit_value += static_cast<unsigned char>(buffer[1]);
        buffer += 2;
        return bit_value;
    }

    void put_16bit(char* buffer, uint16_t value){
        buffer[0] = (value & 0xFF00) >> 8;
        buffer[1] = value & 0xFF;
        buffer += 2;
    }

    uint16_t TID;
    uint16_t BIT_FIELDS; 

    uint16_t QR;
    uint16_t OPCODE;
    uint16_t AA;
    uint16_t TC;
    uint16_t RD;
    uint16_t RA;
    uint16_t Z;
    uint16_t RCODE;

    uint16_t QD_COUNT;
    uint16_t AN_COUNT;
    uint16_t NS_COUNT;
    uint16_t AR_COUNT;

    public:
    void decode_request(char* buffer){

        for(int i = 0; i < 512; i++){
            cout << buffer[i];
        }

        cout << "done" << endl;

        TID =        convert_to_16bit(buffer);
        BIT_FIELDS = convert_to_16bit(buffer);

        QR =      (BIT_FIELDS & QR_MASK);
        OPCODE =  (BIT_FIELDS & OPCODE_MASK);
        AA =      (BIT_FIELDS & AA_MASK);
        TC =      (BIT_FIELDS & TC_MASK);
        RD =      (BIT_FIELDS & RD_MASK);
        RA =      (BIT_FIELDS & RA_MASK);
        RCODE =   (BIT_FIELDS & RCODE_MASK);

        QD_COUNT = convert_to_16bit(buffer);
        AN_COUNT = convert_to_16bit(buffer);
        NS_COUNT = convert_to_16bit(buffer);
        AR_COUNT = convert_to_16bit(buffer);

        encode_header(buffer);

        for(int i = 0; i < 512; i++){
            cout << buffer[i];
        }

        cout << "new" << endl;

    }

    void encode_header(char* buffer){
        put_16bit(buffer, this->TID);
        put_16bit(buffer, this->BIT_FIELDS);
        put_16bit(buffer, this->QD_COUNT);
        put_16bit(buffer, this->AN_COUNT);
        put_16bit(buffer, this->NS_COUNT);
        put_16bit(buffer, this->AR_COUNT);
    }

    void query_show(){
        cout << this->TID
             << this->QR
             << this->OPCODE
             << this->AA
             << this->TC
             << this->RD
             << this->RA
             << this->RCODE
             << this->QD_COUNT
             << this->AN_COUNT
             << this->NS_COUNT
             << this->AR_COUNT;
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