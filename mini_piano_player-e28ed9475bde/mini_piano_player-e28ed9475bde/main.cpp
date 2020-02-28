#include "stdio.h"
#include "ctype.h"
#include "noteplayer.h"
#include "generalinterface.h"
#include "max32630fthr.h"

#define BUFFER_SIZE 128
#define HALF_BUFFER 64
#define OS_MAINSTKSIZE 1024

bool debugState = 0;

MAX32630FTHR pegasus(MAX32630FTHR::VIO_3V3);

PwmOut speaker(P4_0);
AnalogIn POT(AIN_0);
DigitalOut rLED(LED1);
DigitalOut gLED(LED2);
DigitalOut bLED(LED3);
DigitalIn Button(P2_3);

float potval,reading;

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

char *skipToNextEntry(char *songpos, bool skiptype)
{
    if(skiptype == 0) {
        do {
            songpos++;
        } while(!isspace(*(songpos)));
        do {
            songpos++;
        } while(!isalnum(*(songpos)));
        return songpos;
    } else if(skiptype == 1) {
        while(!isspace(*(songpos))) {
            songpos++;
        }
        while(isspace(*(songpos))) {
            songpos++;
        };
    }
    return songpos;
}

bool songParse(note *song,char *buffer)
{
    int candidate;
    char *songpos=buffer+1; //song position
    int pn_det;
    int i;
    for(i=0; i<1024; i++) {
        //take the first base 10 integer you see.
        //this initializes songpos ALWAYS. lol.
        candidate = strtol(songpos-1,&songpos,10);

        if(debugState) {
            printf("Character:%i\r\n",candidate);
        }

        if(candidate == 0) {
            printf("Found invalid NOTE LENGTH value [%c] at position %li. Skipping until next whitespace...\r\n",*songpos,songpos-buffer);
            songpos = skipToNextEntry(songpos,0);
            i--;
            continue;
        } else {
            song[i].length = candidate;
            if(debugState) {
                printf("Entered candidate [%i] into song[%i].length\r\n",candidate,i);
            }
        }

        //parse next character
        if(debugState) {
            printf("Character:%c\r\n",*songpos);
        }

        pn_det=0;

        if(!(*songpos=='#'||'a'<=*songpos<='z'||*songpos=='-')) {
            printf("Found invalid PITCH NAME value [%c] at position %li. Skipping word...\r\n",*songpos,songpos-buffer);
            songpos = skipToNextEntry(songpos,0);
            i--;
            continue;
        }
        if(*songpos=='#') { //watch out for sharps
            pn_det+=1;
            songpos++;
            if(debugState)
            {
                printf("Sharp found.\r\n");
            }
        }
        switch(*songpos) {
            case 'c':
                pn_det+=0;
                break;
            case 'd':
                pn_det+=2;
                break;
            case 'e':
                pn_det+=4;
                break;
            case 'f':
                pn_det+=5;
                break;
            case 'g':
                pn_det+=7;
                break;
            case 'a':
                pn_det+=9;
                break;
            case 'b':
                pn_det+=11;
                break;
            case '-':
                pn_det=57;
                goto skipoctaveparse;
        }

        songpos++;
        if('0'<=*songpos<='9') {
            int num = strtol(songpos,&songpos,10);
            num+=3; //nokia composer octaves are usually much too low.
            if(debugState) {
                printf("octave parsed: %i \t-> adder:%i\r\n",num,((num-2)*12));
            }
            pn_det=pn_det+((num-2)*12);
        } else {
            printf("Found invalid OCTAVE value [%c] at position %li. Skipping word...\r\n",*songpos,songpos-buffer);
            songpos = skipToNextEntry(songpos,0);
            i--; //rewrite current note
            continue;
        }
        skipoctaveparse:
        song[i].pitch = (pitchname)pn_det;
        
        if(debugState) {
            printf("Entered pitch [%i] into song[%i].pitch\r\n",song[i].pitch,i);
            printf("Final: {%i,%i} at i=%i\r\n",song[i].length,song[i].pitch,i);
            printf("-------------------\r\n");
        }

        songpos = skipToNextEntry(songpos,1);

        //check for buffer end
        if(*(songpos) == '\0') {
            i++;
            song[i].length = 1;
            song[i].pitch = END;
            printf("Parsing done. i = %i\r\n",i);
            return 1;
        }
        if(songpos-buffer > 2048) {
            printf("songpos exceeded buffer size.\r\n");
            return 0;
        }
    }
    printf("Song exceeded maximum number of notes.\r\n");
    return 0;
}
void printSong(note *song)
{
    int i;
    for(i=0; song[i].pitch != END; i++) {
        printf("{%i,%i},",song[i].length,song[i].pitch);
    }
    printf("{%i,%i},",song[i].length,song[i].pitch);
    printf("\r\nPrinting done.\r\n");
}

int main()
{

    startFileSystem();

    daplink.printf("\f---daplink SERIAL PORT---\r\n\r\nMINI PIANO PLAYER ver 3 \r\n\r\n\r\n");
    microUSB.printf("micro USB serial port\r\n");
    presence();

    char *title = new char[24];
    title[0] = '/';
    title[1] = 'f';
    title[2] = 's';
    title[3] = '/';
    FILE *txtfile;
    char *buffer = new char[1024];
    while(1) {
        printf("Please input filename: ");
        char *inputptr = title+4;
        if(!getInput(24,inputptr)) {
            printf("Filenames cannot be more than 20 characters.\033[A\r\n");
        }
        txtfile = fopen(title,"r");
        if(txtfile == NULL) {
            printf("File not found. Please append filetype at the end of filename.\033[A\r\n");
            continue;
        }
        break;
    }

    fseek(txtfile,0L,SEEK_END);
    int size = ftell(txtfile);
    fseek(txtfile,0,SEEK_SET);
    fread(buffer,1,size,txtfile);
    printf("Printing file...\r\n");
    {
        int i;
        for(i=0; i<size; i++) {
            printf("%c",buffer[i]);
        }
        buffer[i] = ' ';
        buffer[i+1] = '\0';
        buffer[i+2] = 'a';
    }

    printf("\r\nFile recieved. Parsing...\r\n");

    note song[1024];
    if(!songParse(song,buffer)) {
        printf("Song parse unsuccessful.\r\n");
        return 0;
    }
    printSong(song);
    
    loadBuffer();

    for(int i = 0; 1; i++) {
        if(pitch2freq(song[i].pitch)==-1)
        {
            break;
        }
        playNote(song[i]);
    }
    while(1){
        rLED = !rLED;
        wait(1);
    }
}

