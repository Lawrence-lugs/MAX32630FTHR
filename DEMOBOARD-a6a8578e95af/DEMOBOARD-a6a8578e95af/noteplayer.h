#ifndef _NOTEPLAYER_H_
#define _NOTEPLAYER_H_
#include "mbed.h"

extern float *audioDataBuffer;
extern volatile int bufferPOS;
extern PwmOut speaker;
extern Ticker SampleTime;

void loadBuffer();

typedef enum : unsigned char {
    C2,Cs2,D2,Ds2,E2,F2,Fs2,G2,Gs2,A2,As2,B2,   //C2:0
    C3,Cs3,D3,Ds3,E3,F3,Fs3,G3,Gs3,A3,As3,B3,   //C3:12
    C4,Cs4,D4,Ds4,E4,F4,Fs4,G4,Gs4,A4,As4,B4,   //C4:24
    C5,Cs5,D5,Ds5,E5,F5,Fs5,G5,Gs5,A5,As5,B5,   //C5:36
    C6,Cs6,D6,Ds6,E6,F6,Fs6,G6,Gs6,             //C6:48
    rest,
    END
} pitchname;

typedef struct {
    unsigned char length;
    pitchname pitch;
} note;

int pitch2freq(pitchname pitch2switch);
void placeNewSample();
note makeNote(int length, pitchname pitch2make);
void playNote(note note2play);

#endif