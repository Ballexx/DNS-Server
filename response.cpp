#include <stdint.h>
#include <fstream>
#include <iostream>
#include "response.hpp"

using namespace std;

const uint32_t ZONE_MAX_LEN = 1000;
struct zone{
    string host;
    string TTL;
    string rclass;
    string rtype;
    string rdata;
};
zone zones[ZONE_MAX_LEN];

uint16_t line_index = 0;

string split_data(string line){
    string temp;

    for(;;){
        if(line[line_index] == ' ' || line_index == line.length()){
            line_index += 1;
            return temp;
        }
        temp += line[line_index];
        line_index += 1;
    }  
}

int init_zonedata(const char* path){
    uint32_t zone_index = 0;
    
    string line;
    ifstream zonedata (path);

    if (zonedata.is_open()){
        while (getline(zonedata, line))
        {
            string host    = split_data(line);
            string TTL     = split_data(line);
            string rclass  = split_data(line);
            string rtype   = split_data(line);
            string rdata   = split_data(line);

            line_index = 0;

            zones[zone_index] = {
                host,
                TTL,
                rclass,
                rtype,
                rdata
            };
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
    uint16_t CLASS;
    uint16_t TYPE;
};

char to_char(uint16_t hex_val){
    return static_cast<char>(hex_val);
}

uint16_t buf_index = 0;

void encode_to_buffer(uint16_t value, char* buffer){
    buffer[buf_index+1]  = (value & 0xff);
    buffer[buf_index]  = ((value >> 8) & 0xff);
    buf_index += 2;
}

void Response::build_response(request_data request, char* buffer){    
    buf_index = 0;
    
    encode_to_buffer(request.TRANSACTION_ID, buffer);
    encode_to_buffer(request.BIT_FIELDS, buffer);
    
    if(request.QD_COUNT == 1){
        encode_to_buffer(request.QD_COUNT, buffer);
    }
    else{
        
    }
    
    uint16_t answer_count;
    
    const char* record_type = get_record_type(request.TYPE);
    const char* dns_class   = get_dns_class(request.CLASS);

    for(int i = 0; i < sizeof(zones) / sizeof(zones[0]); i++){
        if(zones[i].rtype != record_type){
            answer_count += 1;
        }
    }

    encode_to_buffer(answer_count, buffer);
    encode_to_buffer(request.NS_COUNT, buffer);
    encode_to_buffer(request.AR_COUNT, buffer);
}
