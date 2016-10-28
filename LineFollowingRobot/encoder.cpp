//
//  encoder.cpp
//  
//
//  Created by Eric Qiu on 2016-10-28.
//
//

#include "encoder.h"
#include "Arduino.h"

volatile static uint8_t PCintLast[3];

volatile static voidFuncPtr PCintFunc[24] = {
    0 };

volatile uint8_t *port_to_pcmask[]= {
    &PCMSK0,
    &PCMSK1,
    &PCMSK2
};

// common code for isr handler. "port" is the PCINT number.
// there isn't really a good way to back-map ports and masks to pins.
static void PCint(uint8_t port);

uint8_t Encoder::_pA = 0;
uint8_t Encoder::_pB = 0;
int32_t Encoder::_position = 0;

/*
 * attach an interrupt to a specific pin using pin change interrupts.
 * First version only supports CHANGE mode.
 */
void PCattachInterrupt(uint8_t pin, void (*userFunc)(void), int mode) {
    uint8_t bit = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);
    uint8_t slot;
    volatile uint8_t *pcmask;
    
    if (mode != CHANGE) {
        return;
    }
    // map pin to PCIR register
    if (port == NOT_A_PORT) {
        return;
    }
    else {
        port -= 2;
        pcmask = port_to_pcmask[port];
    }
    slot = port * 8 + (pin % 8);
    PCintFunc[slot] = userFunc;
    // set the mask
    *pcmask |= bit;
    // enable the interrupt
    PCICR |= 0x01 << port;
}

void PCdetachInterrupt(uint8_t pin) {
    uint8_t bit = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);
    volatile uint8_t *pcmask;
    
    // map pin to PCIR register
    if (port == NOT_A_PORT) {
        return;
    }
    else {
        port -= 2;
        pcmask = port_to_pcmask[port];
    }
    
    // disable the mask.
    *pcmask &= ~bit;
    // if that's the last one, disable the interrupt.
    if (*pcmask == 0) {
        PCICR &= ~(0x01 << port);
    }
}

// common code for isr handler. "port" is the PCINT number.
// there isn't really a good way to back-map ports and masks to pins.
static void PCint(uint8_t port) {
    uint8_t bit;
    uint8_t curr;
    uint8_t mask;
    uint8_t pin;
    
    // get the pin states for the indicated port.
    curr = *portInputRegister(port+2);
    mask = curr ^ PCintLast[port];
    PCintLast[port] = curr;
    // mask is pins that have changed. screen out non pcint pins.
    if ((mask &= *port_to_pcmask[port]) == 0) {
        return;
    }
    // mask is pcint pins that have changed.
    for (uint8_t i=0; i < 8; i++) {
        bit = 0x01 << i;
        if (bit & mask) {
            pin = port * 8 + i;
            if (PCintFunc[pin] != NULL) {
                PCintFunc[pin]();
            }
        }
    }
}

//
// Constructor
//
Encoder::Encoder(uint8_t pA, uint8_t pB) {
    // initialisation of class attributes
    _pA = pA;
    _pB = pB;
    _position = 0;
    
    // initialisation of I/O
    pinMode( _pA, INPUT );
    pinMode( _pB, INPUT );
    digitalWrite( _pA, HIGH );  // pull-up
    digitalWrite( _pB, HIGH );  // pull-up
}

void Encoder::start() {
    PCattachInterrupt( _pA, service, CHANGE );
}

void Encoder::stop() {
    PCdetachInterrupt(_pA);
}

void Encoder::write(int32_t position) {
    _position = position;
}

int32_t Encoder::read(void) {
    return _position;
}

void Encoder::service(void) {
    if ((digitalRead(_pA) == LOW) ^ (digitalRead(_pB) == HIGH)) _position++;
    else _position--;
}
