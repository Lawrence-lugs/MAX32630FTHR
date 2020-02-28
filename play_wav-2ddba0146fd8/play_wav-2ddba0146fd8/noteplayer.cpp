#include "noteplayer.h"

int pitch2freq(pitchname pitch)
{
    switch(pitch) {
        case rest:
            return 0;
        case D2:
            return 73;
        case Ds2:
            return 78;
        case E2:
            return 82;
        case F2:
            return 87;
        case Fs2:
            return 92;
        case G2:
            return 98;
        case Gs2:
            return 104;
        case A2:
            return 110;
        case As2:
            return 117;
        case B2:
            return 123;
        case C3:
            return 131;
        case Cs3:
            return 139;
        case D3:
            return 147;
        case Ds3:
            return 156;
        case E3:
            return 165;
        case F3:
            return 175;
        case Fs3:
            return 185;
        case G3:
            return 196;
        case Gs3:
            return 208;
        case A3:
            return 220;
        case As3:
            return 233;
        case B3:
            return 247;
        case C4:
            return 262;
        case Cs4:
            return 277;
        case D4:
            return 294;
        case Ds4:
            return 311;
        case E4:
            return 330;
        case F4:
            return 349;
        case Fs4:
            return 370;

        case G4:
            return 392;

        case Gs4:
            return 415;

        case A4:
            return 440;

        case As4:
            return 466;

        case B4:
            return 494;

        case C5:
            return 523;

        case Cs5:
            return 554;

        case D5:
            return 587;

        case Ds5:
            return 622;

        case E5:
            return 659;

        case F5:
            return 698;

        case Fs5:
            return 740;

        case G5:
            return 784;

        case Gs5:
            return 831;

        case A5:
            return 880;

        case As5:
            return 932;

        case B5:
            return 988;

        case C6:
            return 1047;

        case Cs6:
            return 1109;

        case D6:
            return 1175;

        case Ds6:
            return 1245;

        case E6:
            return 1319;

        case F6:
            return 1397;

        case Fs6:
            return 1480;

        case G6:
            return 1568;

        case Gs6:
            return 1661;

        case END:
            return -1;
    }
}