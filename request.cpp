#include <stdint.h>
#include <iostream>
#include <cstring>
#include "request.hpp"
#include "response.hpp"

using namespace std;

static const uint16_t QR_MASK       = 0x8000;
static const uint16_t OPCODE_MASK   = 0x7800;
static const uint16_t AA_MASK       = 0x0400;
static const uint16_t TC_MASK       = 0x0200;
static const uint16_t RD_MASK       = 0x0100;
static const uint16_t RA_MASK       = 0x8000;
static const uint16_t RCODE_MASK    = 0x000F;

uint16_t buffer_index;

uint16_t TRANSACTION_ID;
uint16_t BIT_FIELDS; 
uint16_t QR;
uint16_t OPCODE;
uint16_t AA;
uint16_t TC;
uint16_t RD;
uint16_t RA;
uint16_t RCODE;
uint16_t QD_COUNT;
uint16_t AN_COUNT;
uint16_t NS_COUNT;
uint16_t AR_COUNT;
uint16_t QUERY;
uint16_t CLASS;
uint16_t TYPE;

string domain;

uint16_t to_16bit(char* buffer){
    uint16_t first_byte_val  = static_cast<unsigned char>(buffer[buffer_index]);
    uint16_t second_byte_val = static_cast<unsigned char>(buffer[buffer_index + 1]);

    buffer_index += 2;
    return (first_byte_val << 8) | second_byte_val;
};

uint8_t domain_len;

string read_domain(char* buffer){
    string read_data = "";

    for(int i = 0;;i++){
        if(buffer[buffer_index] <= 0x05){
            read_data = read_data + '.';
        }
        else{
            read_data = read_data + buffer[buffer_index];
        }

        domain_len = i;
        buffer_index += 1;

        if(buffer[buffer_index] == 0x00){
            return read_data;
        }
    }
}

void Request::decode_request(char* buffer){
    buffer_index = 0;
    domain = "";

    TRANSACTION_ID = to_16bit(buffer);
    BIT_FIELDS =     to_16bit(buffer);

    QR =      (BIT_FIELDS & QR_MASK);
    OPCODE =  (BIT_FIELDS & OPCODE_MASK);
    AA =      (BIT_FIELDS & AA_MASK);
    TC =      (BIT_FIELDS & TC_MASK);
    RD =      (BIT_FIELDS & RD_MASK);
    RA =      (BIT_FIELDS & RA_MASK);
    RCODE =   (BIT_FIELDS & RCODE_MASK);

    QD_COUNT = to_16bit(buffer);
    AN_COUNT = to_16bit(buffer);
    NS_COUNT = to_16bit(buffer);
    AR_COUNT = to_16bit(buffer);

    buffer_index += 1; //Skip empty byte

    domain = read_domain(buffer);

    buffer_index += 1; //Skip empty byte

    TYPE  = to_16bit(buffer);
    CLASS = to_16bit(buffer);

    memset(buffer, 0, sizeof(buffer));

    Response response;
    response.build_response({
        domain,
        TRANSACTION_ID,
        BIT_FIELDS,
        QR,
        OPCODE,
        AA,
        TC,
        RD,
        RA,
        RCODE,
        QD_COUNT,
        AN_COUNT,
        NS_COUNT,
        AR_COUNT,
        CLASS,
        TYPE
    }, buffer);
}
