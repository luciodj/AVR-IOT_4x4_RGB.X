/*
 * File:   json.h
 * Author: m91329
 *
 * Created on February 14, 2019, 2:42 PM
 */

#ifndef JSON_H
#define	JSON_H

uint8_t * JSON_findQuote(uint8_t *p);
uint8_t * JSON_getValue(uint8_t *json, char* key);
uint32_t JSON_getInt(uint8_t *json, char* key);


#endif	/* JSON_H */

