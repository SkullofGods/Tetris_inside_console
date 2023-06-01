//
// Created by skullofgods on 29.05.23.
//
#include <iostream>
#include <termios.h>
#include "IO.h"


int IO::GetKey(){
    struct termios term;
    tcgetattr(1, &term);
    term.c_lflag &= ~ICANON;
    tcsetattr(1, TCSANOW, &term);
    char k,e,y;
    std::cin >> k;
    std::cin >> e;
    std::cin >> y;
    if ((k==27)&&(e=91)) {
        switch (y) {
            case 65:
                return 1;
                break;
            //UpArrow;
            case 66:
                return 2;
                break;
            //DownArrow;
            case 67:
                return 3;
                break;
            //RightArrow;
            case 68:
                return 4;
                break;
            //LeftArrow;
        }

    }
    return 0;
}


