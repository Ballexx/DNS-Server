#include <stdint.h>
#include <fstream>
#include <iostream>
#include "response.hpp"

using namespace std;

const uint32_t ZONE_MAX_LEN = 1000;
string zones[ZONE_MAX_LEN];

int init_zonedata(const char* path){
    uint32_t zone_index = 0;
    
    string line;
    ifstream zonedata (path);

    if (zonedata.is_open()){
        while ( getline (zonedata,line) )
        {
            zones[zone_index] = line;
            zone_index += 1;
        }
        zonedata.close();
        return 0;
    }
    cout << "Unable to open file";
    
    return 1;
}

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
const char* get_dns_class(uint16_t value){
    switch(value){
        case 0x01:
            return "IN";
        case 0x02:
            return "Unassigned";
        case 0x03:
            return "CH";
        case 0xFF:
            return "ANY";
    }
    return "UNKNOWN CLASS";
}

struct request_data{
    string   domain;
    uint16_t TRANSACTION_ID;
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
    uint16_t CLASS;
    uint16_t TYPE;
};

char to_char(uint16_t hex_val){
    return static_cast<char>(hex_val);
}

void Response::build_response(request_data request){

    cout << zones[0] << endl;

    const char* record_type = get_record_type(request.TYPE);
    const char* dns_class   = get_dns_class(request.CLASS);
}
