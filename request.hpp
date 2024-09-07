#include <stdint.h>
#include <iostream>
#pragma once

class Request{
    public:
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

    void decode_request(char* buffer);
};