//
//  encoder.h
//  
//
//  Created by Eric Qiu on 2016-10-28.
//
//

#ifndef encoder_h
#define encoder_h

#include "pins_arduino.h"

typedef void (*voidFuncPtr)(void);


/*
 * attach an interrupt to a specific pin using pin change interrupts.
 * First version only supports CHANGE mode.
 */
void PCattachInterrupt(uint8_t pin, void (*userFunc)(void), int mode);
void PCdetachInterrupt(uint8_t pin);



class Encoder {
private:
    static uint8_t _pA, _pB;
    static int32_t _position;
    static void service(void);
public:
    // public methods
    Encoder(uint8_t, uint8_t);
    void start();
    void stop();
    void write(int32_t);
    int32_t read(void);
};

#endif /* encoder_hpp */
