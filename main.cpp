#include <iostream>
#include <thread>
#include <chrono>
#include "IO.cpp"
#include "tetraminoes.cpp"

#define CYAN "\e[1;36m"
#define GREEN "\e[1;31m"
#define RedBck "\e[40m"
#define CLEAN "\e[0m"
#define BLACK "\e[1;30m"
#define WIDTH 14
#define HEIGHT 22
#define GAP 2

Tetraminoes pTetra;
IO mIO;
void Create();
void Output();
void GetNextBlock();
void WriteGlass(int x, int y);
void WriteStatic();
void CheckForDelete();

bool isAvailibleMove(int curX, int curY, int type, int rot, int moveX, int moveY);
bool isAvailibleRot(int curX, int curY, int type, int rot, int dest);

//threads
void Input();
void Fall();
void GameOver();

bool isGameOver = false;
int glass[HEIGHT][WIDTH];
int glassStatic[HEIGHT][WIDTH];
int nextBlock[2]={7,7};
int score;
int _curX = 5;
int _curY = 0;
int timer = 1000;



int main() {
    Create();
    GetNextBlock();
    WriteGlass(_curX, _curY);
    Output();
    std::thread t1(Input);
    std::thread t2(Fall);
    std::thread t3(GameOver);
    t2.join();
    t1.join();
    t3.join();
}


void Create(){
    for (int i = 0; i<HEIGHT; i++){
        for (int j = 0; j<WIDTH; j++){
            if(j<GAP||j>WIDTH-3||i==HEIGHT-1){
                glass[i][j]=6;
            } else glass[i][j]=0;
        }
    }
    for (int i = 0; i<HEIGHT; i++){
        for (int j = 0; j<WIDTH; j++){
            if(j<GAP||j>WIDTH-3||i==HEIGHT-1){
                glassStatic[i][j]=6;
            } else glassStatic[i][j]=0;
        }
    }
}

void Output(){
    std::system("clear");
    for (int i=GAP; i < HEIGHT; i++){
        for(int j=0; j<WIDTH; j++) {
            if(glass[i][j]==0){
                std::cout << BLACK RedBck " ■";
            }else if (glass[i][j]==1){
                std::cout << CYAN RedBck " ■";
            }else if (glass[i][j]==6){
                std::cout << GREEN RedBck " ■";
            }
        }
        std::cout << RedBck " " CLEAN << std::endl;
    }
    std::cout << std::endl << "Your Score: " << score << std::endl;
}

void GetNextBlock(){
    int type = time(NULL)%7;
    int rot = time(NULL)%4;
    nextBlock[0] = type;
    nextBlock[1] = rot;
}

bool isAvailibleMove(int curX, int curY, int type, int rot, int moveX, int moveY){
    int counter=0;
    for(int y=0; y<5; y++){
        for(int x=0; x<5; x++){
            if(glassStatic[y+curY+moveY][x+curX+moveX]+pTetra.GetBlock(type,rot,y,x)>1&&glassStatic[y+curY+moveY][x+curX+moveX]+pTetra.GetBlock(type,rot,y,x)!=6){
                counter++;
            }
        }
    }
    if(counter>0)
        return false;
    else
        return true;
}

bool isAvailibleRot(int curX, int curY, int type, int rot, int dest){
    int counter=0;
    for(int y=0; y<5; y++){
        for(int x=0; x<5; x++){
            if(glassStatic[curY+y][curX+x]+pTetra.GetBlock(type,rot+dest,y,x)>1&&glassStatic[curY+y][curX+x]+pTetra.GetBlock(type,rot+dest,y,x)!=6){
                counter++;
            }
        }
    }
    if(counter>0)
        return false;
    else
        return true;
}

void WriteGlass(int x, int y){
    for (int i = 0; i<HEIGHT; i++){
        for (int j = 0; j<WIDTH; j++){
            glass[i][j] = glassStatic[i][j];
        }
    }
    for(int curY=0; curY<5; curY++){
        for(int curX=0; curX<5; curX++){
            glass[y+curY][x+curX]+=pTetra.GetBlock(nextBlock[0], nextBlock[1], curY, curX);
        }
    }
}

void WriteStatic(){
    for (int i = 0; i<HEIGHT; i++){
        for (int j = 0; j<WIDTH; j++){
            glassStatic[i][j] = glass[i][j];
        }
    }
    CheckForDelete();
    if(score > 80000)
        timer = 200;
    else if(score > 60000)
        timer = 300;
    else if(score > 40000)
        timer = 400;
    else if(score > 20000)
        timer = 500;
    else if (score >10000)
        timer = 600;
    else if (score > 5000)
        timer = 800;
    else
        timer = 1000;

    if(glassStatic[2][5]!=0||glassStatic[2][6]!=0||glassStatic[2][7]!=0){
        isGameOver = true;
    }
}

void CheckForDelete(){
    int linesCounter=0;
    for (int i = HEIGHT-2; i>0; i--){
        int onesCounter=0;
        for (int j = GAP; j<WIDTH-GAP; j++){
            if (glassStatic[i][j]!=0){
                onesCounter++;
            }
            if(onesCounter >9){
                linesCounter++;
                for (int l = GAP; l<WIDTH-GAP; l++){
                    glassStatic[i][l]=0;
                }
                for(int a = i; a>0; a--){
                        for(int b=GAP; b< WIDTH-GAP; b++){
                            int temp;
                            temp = glassStatic[a][b];
                            glassStatic[a][b] = glassStatic[a-1][b];
                            glassStatic[a-1][b]=temp;
                        }
                }
                i++;
            }
        }
    }
    if(linesCounter==1)
        score+=40;
    else if(linesCounter==2)
        score+=100;
    else if(linesCounter==3)
        score+=300;
    else if(linesCounter==4)
        score+=1000;
    linesCounter = 0;
};

void Input() {
    while (!isGameOver) {
        switch (mIO.GetKey()) {
            case 1: //Up
                if (nextBlock[1] == 3) {
                    if (isAvailibleRot(_curX, _curY, nextBlock[0], nextBlock[1], -3))
                        nextBlock[1] = 0;
                } else if (isAvailibleRot(_curX, _curY, nextBlock[0], nextBlock[1], 1))
                    nextBlock[1]++;
                WriteGlass(_curX, _curY);
                Output();
                break;
            case 2: //Down
                if (isAvailibleMove(_curX, _curY, nextBlock[0], nextBlock[1], 0, 1)) {
                    _curY++;
                    WriteGlass(_curX, _curY);
                }
                Output();
                break;
            case 3: //Right
                if (isAvailibleMove(_curX, _curY, nextBlock[0], nextBlock[1], 1, 0)) {
                    _curX++;
                    WriteGlass(_curX, _curY);
                }
                Output();
                break;
            case 4: //Left
                if (isAvailibleMove(_curX, _curY, nextBlock[0], nextBlock[1], -1, 0)) {
                    _curX--;
                    WriteGlass(_curX, _curY);
                }
                Output();
                break;
            default:
                continue;
        };
    }
}
void Fall(){
    while (!isGameOver) {
        std::this_thread::sleep_for(std::chrono::milliseconds(timer));
        if (!isAvailibleMove(_curX, _curY, nextBlock[0], nextBlock[1], 0, 1)) {
            WriteStatic();
            GetNextBlock();
            _curX = 5;
            _curY = 0;
            WriteGlass(_curX, _curY);
        } else {
            _curY++;
            WriteGlass(_curX, _curY);
        }
        Output();
    }
}

void GameOver(){
    while(true){
        if(isGameOver){
            std::system("clear");
            std::cout << "Game over" << std::endl << "Your score: " << score;
            break;
        }
    }
}
