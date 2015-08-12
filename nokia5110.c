/**
 * Implementation of graphics.h which works for a Nokia 5110 display.
 */

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif
#include <stdint.h>

//TODO Need a way of bringing this in as a dependency as it defines delay5ms
#include "utils.h"
#include "graphics.h"


#define DIR_RESET TRISBbits.TRISB1
#define DIR_DC TRISBbits.TRISB3
#define PIN_RESET LATBbits.LATB1
#define PIN_DC    LATBbits.LATB3

#define COMMAND  0
#define DATA 1

#define LCD_WIDTH     84
#define LCD_HEIGHT     48
#define LCD_BLOCK_HEIGHT  6


void staticnokia5110_sendByteOverSpi(uint8_t byte) {
    SSP1BUF = byte;
    while (!PIR1bits.SSP1IF) {
        //Do nothing.
    }
    PIR1bits.SSP1IF = 0;
}

void staticnokia5110_sendByte(uint8_t byte, uint8_t command) {
    PIN_DC = command;
   nokia5110_sendByteOverSpi(byte);
}

/**
 * Goto the given location.
 *
 * Note that the vertical space is split into 6 blocks which are 8 pixels
 * high.
 *
 * @param x Horizontal location: 0 <= x <= 83
 * @param y Vertical location: 0 <= y <= 5
 */
void staticnokia5110_goto(uint8_t x, uint8_t y) {
   nokia5110_sendByte(0x40 | y, COMMAND);
   nokia5110_sendByte(0x80 | x, COMMAND);
}


void staticnokia5110_setupSpi() {
    //Set the data and clock pins to output.
    TRISBbits.TRISB1 = 0;
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB4 = 0;

    //Transmit on falling clock
    SSP1STATbits.SMP = 0;
    SSP1STATbits.CKE = 1;

    //Clock is active high
    SSP1CON1bits.CKP = 0;

    //SPI master mode using clock speed / 4
    SSP1CON1bits.SSPM3 = 0;
    SSP1CON1bits.SSPM2 = 0;
    SSP1CON1bits.SSPM1 = 0;
    SSP1CON1bits.SSPM0 = 0;

    //Enable SPI
    SSP1CON1bits.SSPEN = 1;
    PIE1bits.SSP1IE = 1;
    INTCONbits.PEIE = 1;
}

void graphics_initialise() {
    DIR_RESET = 0;
    DIR_DC = 0;
    PIN_RESET = 1;
    PIN_DC = COMMAND;

    delay5ms(5);
    PIN_RESET = 0;
    delay5ms(5);
    PIN_RESET = 1;

   nokia5110_setupSpi();

   nokia5110_sendByte(0x20 + 0x01, COMMAND); // Extended instructions enabled
   nokia5110_sendByte(0x80 + 0x40, COMMAND); // Set contrast 0 - 127
   nokia5110_sendByte(0x04 + 0x02, COMMAND); // Temperature control
   nokia5110_sendByte(0x10 + 0x03, COMMAND); // Set bias system
   nokia5110_sendByte(0x20 + 0x00, COMMAND); // Return to basic instruction set, power on, set horizontal addressing
   nokia5110_sendByte(0x08 + 0x04, COMMAND); // Display control set to normal mode
}


void graphics_drawBitmap(point_t position, uint8_t * data, uint8_t bitmapWidth, uint16_t dataLength)
{
    uint8_t currentY = position.y;

   nokia5110_goto(position.x,position.y);

    while (dataLength)
    {
        uint8_t width = bitmapWidth;
        while (width)
        {
           nokia5110_sendByte(* data, DATA);
            dataLength--;
            width--;
            data++;
        }
       nokia5110_goto(position.x, ++currentY);
    }
}

void graphics_clear()
{
   nokia5110_goto(0,0);

    uint16_t clearLength = LCD_BLOCK_HEIGHT * LCD_WIDTH;
    while (clearLength--) {
       nokia5110_sendByte(0x0, DATA);
    }
}

void graphics_computeNextXPositionAfterBitmap(point_t * position, bitmap_t bitmap)
{
    position->x = position->x + bitmap.width;
}
