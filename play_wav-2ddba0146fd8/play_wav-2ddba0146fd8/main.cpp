#include "mbed.h"
#include "max32630fthr.h"
#include "stdio.h"
#include "ctype.h"
#include "generalinterface.h"
#define BUFFER_SIZE 1024
#define HALF_BUFFER 512

bool debugState = 1;

DigitalOut rLED(LED1);
DigitalOut gLED(LED2);
DigitalOut bLED(LED3);

DigitalIn Button(P2_3);

MAX32630FTHR pegasus(MAX32630FTHR::VIO_3V3);
PwmOut PWM(P4_0);
AnalogIn POT(AIN_0);
int i;
volatile int bufferPOS = 0;
short audioDataBuffer[BUFFER_SIZE];
struct WavFile {
    long int size;
    int channels;
    int sampleRate;
    int bitsPerSample;
};

float maxSampleVal=1;
WavFile Track;
Ticker SampleTime;

DigitalIn db1(P3_3);
float potval;

void placeNewSample(void)
{
    PWM.write((((float)audioDataBuffer[bufferPOS]*potval)/maxSampleVal)+0.5); //multiply by POT value for volume.
    bufferPOS = (bufferPOS+2)%BUFFER_SIZE;
    //if(!Button)
    //{
    //    SampleTime.detach();
    //}
    //printf("%i/%f = %f\r\n",audioDataBuffer[bufferPOS],(maxSampleVal),((float)audioDataBuffer[bufferPOS])/maxSampleVal)+1;
}

void presence()
{
    rLED = LED_ON;
    wait_ms(500);
    rLED = LED_OFF;
    gLED = LED_ON;
    wait_ms(500);
    bLED = LED_ON;
    gLED = LED_OFF;
}

int main()
{
    startFileSystem();

    daplink.printf("\f---DAPLINK SERIAL PORT---\r\n\r\nWAV PLAYER VER 2\r\n\r\n");
    microUSB.printf("micro USB serial port\r\n");

    presence();

restart:

    char *title = new char[24];
    title[0] = '/';
    title[1] = 'f';
    title[2] = 's';
    title[3] = '/';
    FILE *audio;
    while(1) {
        printf("Please input filename: ");
        char *inputptr = title+4;
        if(!getInput(24,inputptr)) {
            printf("Filenames cannot be more than 20 characters.\r\n");
        }
        audio = fopen(title,"r");
        if(audio == NULL) {
            printf("File not found. Please append filetype at the end of filename.\r\n");
            continue;
        }
        break;
    }
    printf("\r\nFile opened.\r\n\r\n");

    //find file size
    fseek(audio,0L,SEEK_END);
    Track.size = ftell(audio);

    printf("File Size:%li \r\n",Track.size);

    //read sample rate and channels of wav file
    fseek(audio,22,SEEK_SET);
    fread(&Track.channels, 2, 1,audio);
    fread(&Track.sampleRate, 2, 1,audio);
    printf("Sample Rate: %i\r\n",Track.sampleRate);
    printf("%i channels \r\n",Track.channels);
    if(Track.sampleRate >= 20000) {
        printf("WARNING: Microcontroller may not be able to play sample rates higher than 20kHz properly.\r\n");
    }

    //read bits per sample
    fseek(audio,34,SEEK_SET);
    fread(audioDataBuffer, 2, 1,audio);
    Track.bitsPerSample = (audioDataBuffer[1] << 8) + audioDataBuffer[0];
    printf("Bits Per Sample: %i\r\n",Track.bitsPerSample);
    for(int i=0;i<Track.bitsPerSample;i++)
    {
        maxSampleVal *= 2;
    }
    maxSampleVal/=2;

    int flag;
    //find start of actual audio
    fseek(audio,44,SEEK_SET);

    printf("Size: %i bytes\r\n",Track.size);

    PWM.period_us(20);
    short size = sizeof(short);
    
    fread((void *)audioDataBuffer,size,HALF_BUFFER,audio);
    printf("Sample time will be attached with period %f.",1.0/(double)(Track.sampleRate));
    SampleTime.attach(&placeNewSample,1.0/(double)(Track.sampleRate));
    //microcontroller may not be strong enough to call fread after attaching a 44.1kHz wav file.
    printf("\r\nTicker attached.\r\nRunning buffer loop...\r\n\r\n");

    //take the first block of audio data into the buffer (buffer size is two blocks)
    flag = 0;
    while(ftell(audio) < Track.size) {
        potval = POT.read()*2;
        if((bufferPOS < HALF_BUFFER) && flag == 0) {
            fread((void *)(audioDataBuffer + HALF_BUFFER),size,HALF_BUFFER,audio);
            flag = !flag;
        } else if((bufferPOS >= HALF_BUFFER) && flag == 1) {
            fread((void *)audioDataBuffer,size,HALF_BUFFER,audio);
            flag = !flag;
        }
    }
    SampleTime.detach();
    bLED=LED_OFF;
    rLED=LED_ON;
    while(1)
    {
        if(!db1)
        {
            goto restart;
        }
    }
};

