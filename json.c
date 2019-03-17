/*
\file   json.c

\brief  A simple JSON string parser

(c) 2018 Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software and any
derivatives exclusively with Microchip products. It is your responsibility to comply with third party
license terms applicable to your use of third party software (including open source software) that
may accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS
FOR A PARTICULAR PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT
OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS
SOFTWARE.
*/
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "mcc_generated_files/cryptoauthlib/lib/basic/atca_helpers.h"

uint8_t * JSON_findQuote(uint8_t *p) {
    while(*p) {
        if (*p == '"') return p;    // return pointer AFTER \"
        if (*p == '\\') {
            p++;
            if (*p == '\0') break;// '\' was last char in string
        }
        p++;
    }
    return p;    // encountered end of string
}

// advance pointer skipping white spaces
//
uint8_t * next(uint8_t *p) {
    while (*p && isWhiteSpace(*p)) p++;
    return p;
}

// search for a "key" inside a JSON object
//      return a pointer to value string (open quote or first alphanum)
//      return NULL if not found
uint8_t * JSON_getValue(uint8_t *json, char* key){
    uint8_t *p = json;    // point immediately to first "

    while( *p != '\0') { // loop over json object key pairs
        p = JSON_findQuote(p);
        if (strncmp((char*)p+1, key, strlen(key)) == 0) {
            p += 1+ strlen(key);
            if (*p == '"') { // key must be in quotes -> it is a match
                p = next(p+1);            // skip whitespace
                if (*p != ':') break;   // key must be followed by ':' -> bad json
                p = next(p+1);            // skip whitespace
                if ((*p == '\0')||(*p == '}')) break; // value missing
                return p;   // return pointing to the value
            }
        }
        // not a match, find the next key pair
        p = JSON_findQuote(p+1); // skip past the key
        if (*p == '\0') break;  // key must be in quotes -> bad json
        p = next(p+1);            // skip whitespace
        if (*p != ':') break;   // key must be followed by ':' -> bad json
        p = next(p+1);            // skip whitespace
        if ((*p == '\0')||(*p == '}')) break; // value missing
        if (*p == '"') {        // skip past string value
            p = JSON_findQuote(p+1);
            if (*p == '\0') break;
            p = next(p+1);            // skip white spaces
            }
        else {
            while(isDigit(*p)) p++;
        }
        p = next(p);            // skip white spaces
        if (*p != ',') break;     // expecting a ',' -> not found or bad json
    }
    return NULL;
}

inline static uint8_t hex_digit_to_num(char c)
{
    if (c >= '0' && c <= '9') {
        return (uint8_t)(c - '0');
    }
    if (c >= 'a' && c <= 'f') {
        return (uint8_t)(c - 'a') + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return (uint8_t)(c - 'A') + 10;
    }
    return 16;
}

uint32_t get_number(uint8_t *p){
    uint32_t number = 0;

    if ((p[0] == '"') &&  (p[1] != '\0'))    // if inside a string, (i.e. slider)
        p++;

    if ((p[0] == '0') && (p[1] == 'x')) {    // if hexadecimal (i.e. RGB value)
        p += 2;
        while (*p && isHexDigit(*p)) {
            number <<= 4;
            number += hex_digit_to_num(*p);
            p++;
            }
    } else {
        while (*p && isDigit(*p)) {
            number *= 10;
            number += (*p - '0');
            p++;
        }
    }
    return number;
}

uint32_t JSON_getInt(uint8_t *p, char * key) {
    uint8_t *pV = JSON_getValue(p, key);
    if (pV != NULL) {
        return get_number(pV);
    }

    return -1;
}
