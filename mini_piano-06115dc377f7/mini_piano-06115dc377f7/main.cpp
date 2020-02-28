#include "mbed.h"
#include "max32630fthr.h"
#include "USBSerial.h"
#include "stdio.h"
#include "SDFileSystem.h"
#include "ctype.h"

//still needs BITS PER SAMPLE PARSING.
//do this like so: PWM.write(WavValue/2^BPS)

//MAPPING IS WRONG. P = 440 PLZ. Y = MIDDLE D.

#define BUFFER_SIZE 128
#define HALF_BUFFER 64

DigitalOut rLED(LED1);
DigitalOut gLED(LED2);
DigitalOut bLED(LED3);

DigitalIn Button(P2_3);

MAX32630FTHR pegasus(MAX32630FTHR::VIO_3V3);
PwmOut PWM(P4_0);
AnalogIn POT(AIN_0);
volatile int bufferPOS = 0;
volatile unsigned int g=0;

Serial daplink(P2_1,P2_0);
USBSerial microUSB;
SDFileSystem sd(P0_5, P0_6, P0_4, P0_7, "sd");

float audioDataBuffer[BUFFER_SIZE];

struct WavFile
{
    long int size;
    int channels;
    int sampleRate;
    int bitsPerSample;
};

float potval,reading;

void placeNewSample(void)
{
    PWM.write(audioDataBuffer[bufferPOS++]); //multiply by POT value for volume.
    bufferPOS = (bufferPOS+1) & 0x07F;
}

int main()
{   
    WavFile Track;
    Ticker SampleTime;
    
    daplink.printf("\f---DAPLINK SERIAL PORT---\r\n\r\nMINI PIANO ver 1 \r\nMAKE SURE YOUR PUTTY IS SET TO NOT WAIT FOR [ENTER] \r\nMIDDLE A (A4) (440Hz) IS Y, [WHITE] KEYS VALID FROM 1 to L. \r\nPRESS SHIFT FOR SHARP\r\n\r\n");
    microUSB.printf("micro USB serial port\r\n");
    rLED = LED_ON;
    wait_ms(500);
    rLED = LED_OFF;
    gLED = LED_ON;
    wait_ms(500);
    bLED = LED_ON;
    gLED = LED_OFF;
    
    printf("Generating sine...\r\n");
    int i;
    for(i=0; i<128;i++)
    {
        audioDataBuffer[i] =((1.0 + sin((double(i)/16.0*6.28318530717959)))/2.0); //formula copied from mbed example
    }
    
    printf("Ready for input.\r\n");
    
    int PlayingFreq = 440, attached = 0;
    
    while(1)
    {
    
        Track.sampleRate = PlayingFreq * 16; //TONE FREQ = SAMPLE RATE / SAMPLES PER CYCLE
        
        PWM.period_us(1); //1MHz
        
        float ticker_period = (float) 1/(Track.sampleRate);
        
        printf("\r\nTicker Period: %f\tTicker Freq: %f\r\nTarget Freq: %i \r\n\r\n",ticker_period, 1/ticker_period, PlayingFreq);

        if(attached)
        {
            SampleTime.attach(&placeNewSample,ticker_period);
        }
        
        bLED = LED_OFF;
        
        char c;
        c = daplink.getc();
        
        gLED = LED_ON;
        
        switch(c)
        {
            case '1':
                PlayingFreq = 73;
                break;
            case '!':
                PlayingFreq = 78;
                break;
            case '2':
                PlayingFreq = 82;
                break;
            case '3':
                PlayingFreq = 87;
                break;
            case '#':
                PlayingFreq = 92;
                break;
            case '4':
                PlayingFreq = 98;
                break;
            case '$':
                PlayingFreq = 104;
                break;
            case '5':
                PlayingFreq = 110;
                break;
            case '%':
                PlayingFreq = 117;
                break;
            case '6':
                PlayingFreq = 123;
                break;
            case '7':
                PlayingFreq = 131;
                break;
            case '8':
                PlayingFreq = 139;
                break;
            case '9':
                PlayingFreq = 147;
                break;
            case '(':
                PlayingFreq = 156;
                break;
            case '0':
                PlayingFreq = 165;
                break;
            case 'q':
                PlayingFreq = 175;
                break;
            case 'Q':
                PlayingFreq = 185;
                break;
            case 'w':
                PlayingFreq = 196;
                break;
            case 'W':
                PlayingFreq = 208;
                break;
            case 'e':
                PlayingFreq = 220;
                break;
            case 'E':
                PlayingFreq = 233;
                break;
            case 'r':
                PlayingFreq = 247;
                break;
            case 't':
                PlayingFreq = 262;
                break;
            case 'T':
                PlayingFreq = 277;
                break;
            case 'y':
                PlayingFreq = 294;
                break;
            case 'Y':
                PlayingFreq = 311;
                break;
            case 'u':
                PlayingFreq = 330;
                break;
            case 'i':
                PlayingFreq = 349;
                break;
            case 'I':
                PlayingFreq = 370;
                break;
            case 'o':
                PlayingFreq = 392;
                break;
            case 'O':
                PlayingFreq = 415;
                break;
            case 'p':
                PlayingFreq = 440;
                break;
            case 'P':
                PlayingFreq = 466;
                break;
            case 'a':
                PlayingFreq = 494;
                break;
            case 's':
                PlayingFreq = 523;
                break;
            case 'S':
                PlayingFreq = 554;
                break;
            case 'd':
                PlayingFreq = 587;
                break;
            case 'D':
                PlayingFreq = 622;
                break;
            case 'f':
                PlayingFreq = 659;
                break;
            case 'g':
                PlayingFreq = 698;
                break;
            case 'G':
                PlayingFreq = 740;
                break;
            case 'h':
                PlayingFreq = 784;
                break;
            case 'H':
                PlayingFreq = 831;
                break;
            case 'j':
                PlayingFreq = 880;
                break;
            case 'J':
                PlayingFreq = 932;
                break;
            case 'k':
                PlayingFreq = 988;
                break;
            case 'l':
                PlayingFreq = 1047;
                break;
            case 'L':
                PlayingFreq = 1109;
                break;
            case 'z':
                PlayingFreq = 1175;
                break;
            case 'Z':
                PlayingFreq = 1245;
                break;
            case 'x':
                PlayingFreq = 1319;
                break;
            case 'c':
                PlayingFreq = 1397;
                break;
            case 'C':
                PlayingFreq = 1480;
                break;
            case 'v':
                PlayingFreq = 1568;
                break;
            case 'V':
                PlayingFreq = 1661;
                break;
            case '\r':
                attached = !attached;
                gLED = !gLED;
                break;
        }

        gLED = LED_OFF;
        rLED = !rLED;
        SampleTime.detach();
        
        printf("\033[A\033[A\033[A\033[A");
    }
};

