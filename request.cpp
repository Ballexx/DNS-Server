#include <stdint.h>
#include <iostream>
#include "request.hpp"

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
uint16_t Z;
uint16_t RCODE;
uint16_t QD_COUNT;
uint16_t AN_COUNT;
uint16_t NS_COUNT;
uint16_t AR_COUNT;
uint16_t QUERY;
uint16_t CLASS;
uint16_t TYPE;

uint16_t to_16bit(char* buffer){
    uint16_t first_byte_val  = static_cast<unsigned char>(buffer[buffer_index]);
    uint16_t second_byte_val = static_cast<unsigned char>(buffer[buffer_index + 1]);

    buffer_index += 2;
    return (first_byte_val << 8) | second_byte_val;
};

const char* get_record_type(uint16_t value){
    switch(value){
        case 0x01:
            return "A";
        case 0x1c:
            return "AAAA";
        case 0x0c:
            return "AFSDB";
        case 0x02:
            return "NS";
        case 0x05:
            return "CNAME";
        case 0x0F:
            return "MX";
    }
    return "UNKNOWN RECORD";
}

void Request::decode_request(char* buffer){
    buffer_index = 0;
    uint8_t domain_len;

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

    buffer_index += 1; //Skip starting byte for query

    string domain;

    for(int i = 0;;i++){
        if(buffer[buffer_index] == 0x04 || buffer[buffer_index] == 0x03){ //Hexvalue of byte for separator of n-level domain is 4 whilst 3 for top level domain
            domain = domain + '.';
        }
        else{
            domain = domain + buffer[buffer_index];
        }

        domain_len = i;
        buffer_index += 1;

        if(buffer[buffer_index] == 0x00){
            break;
        }
    }
    buffer_index += 1; //Skip empty byte

    TYPE  = to_16bit(buffer);
    CLASS = to_16bit(buffer);

    const char* record_type = get_record_type(TYPE);

    cout << domain << endl;
    cout << record_type << endl;
}