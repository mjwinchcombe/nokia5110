/*
 * File:   graphics.h
 * Author: mstthew
 *
 * Created on 13 April 2014, 17:51
 */

#ifndef GRAPHICS_H
#define	GRAPHICS_H


typedef struct {
    uint8_t x;
    uint8_t y;
} point_t;


//TODO bitmap might need to be moved to its own header
typedef struct {
    uint8_t width;
    uint16_t dataLength;
    const uint8_t * data;
} bitmap_t;



void graphics_initialise(void);

void graphics_computeNextXPositionAfterBitmap(point_t * previousPosition, bitmap_t bitmap);

void graphics_drawBitmap(point_t startPosition, uint8_t * data, uint8_t bitmapWidth, uint16_t dataLength);

void graphics_clear(void);

#endif	/* GRAPHICS_H */
