#include "mbed.h"
#include "max32630fthr.h"
#include "USBSerial.h"

MAX32630FTHR pegasus(MAX32630FTHR::VIO_3V3);

Serial daplink(P2_1, P2_0);
USBSerial microUSB;

DigitalOut rLED(LED1);
DigitalOut gLED(LED2);
DigitalOut bLED(LED3);

DigitalIn Button(P2_3);

PwmOut led_1(P5_6);
PwmOut led_2(P5_5);
PwmOut led_3(P5_4);
PwmOut led_4(P5_3);

AnalogIn POT(AIN_0);

void PwmInit()
{
    led_1.period_ms(10);
    led_1 = 0.5f;
    led_2.period_ms(10);
    led_2 = 0.5f;
    led_3.period_ms(10);
    led_3 = 0.5f;
    led_4.period_ms(10);
    led_4 = 0.5f;
}

// main() runs in its own thread in the OS
// (note the calls to Thread::wait below for delays)
int main()
{

    daplink.printf("\f---DAPLINK SERIAL PORT---\r\n\r\nPWM LIGHTS VER 1 (POLLING)\r\n\r\n");
    microUSB.printf("micro USB serial port\r\n");
    rLED = LED_ON;
    wait_ms(500);
    rLED = LED_OFF;
    gLED = LED_ON;
    wait_ms(500);
    bLED = LED_ON;
    gLED = LED_OFF;

    daplink.printf("Initializing PWM...\r\n");
    PwmInit();

    float state;
    daplink.printf("Entering loop...\r\n\r\n\r\n");
    gLED = LED_ON;
    rLED = LED_OFF;
    char fakebuffer[100];
    bLED = LED_OFF;
    while(1) {
        gLED=!gLED;
        rLED=!rLED;            
        while(1) {
            state = POT.read();
            printf("Controlling LED1\tPOT_VAL:%f\r",state);
            led_1.write(state);
            wait_ms(50);
            if(!Button.read()) {
                wait_ms(500);
                break;
            }
        }
        gLED=!gLED;
        rLED=!rLED;    
        while(1) {
            state = POT.read();
            printf("Controlling LED2\tPOT_VAL:%f\r",state);
            led_2.write(state);
            wait_ms(50);
            if(!Button.read()) {
                wait_ms(500);
                break;
            }
        }
        gLED=!gLED;
        rLED=!rLED;
        printf("In LED3.\r");
        while(1) {
            state = POT.read();
            printf("Controlling LED3\tPOT_VAL:%f\r",state);
            led_3.write(state);
            wait_ms(50);
            if(!Button.read()) {
                wait_ms(500);
                break;
            }
        }
        gLED=!gLED;
        rLED=!rLED;
        printf("In LED4.\r");
        while(1) {
            state = POT.read();
            printf("Controlling LED4\tPOT_VAL:%f\r",state);
            led_4.write(state);
            wait_ms(50);
            if(!Button.read()) {
                wait_ms(500);
                break;
            }
        }
        gLED=!gLED;
        rLED=!rLED;
    }
}

