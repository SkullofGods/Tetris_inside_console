#include <iostream>
#include <thread>
#include <chrono>
#include "IO.cpp"
#include "tetraminoes.cpp"
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

#define EMPTY "\e[0;37m"
#define BLOCK "\e[1;36m"
#define A_BLOCK "\e[1;32m"
#define B_BLOCK "\e[1;35m"
#define C_BLOCK "\e[1;33m"
#define D_BLOCK "\e[1;34m"
#define WALL "\e[1;31m"
#define BACKGROUND "\e[40m"
#define CLEAN "\e[0m"
#define WIDTH 14
#define HEIGHT 22
#define GAP 2

//isMultiplayer
int buffer[22][14];
int enScore[1];
int server_fd, new_socket;
int game_over[1] = {0};
int enemy_game_over[1] = {0};
bool isMultiplayer = false;

Tetraminoes pTetra;
IO mIO;
void Create();
void Output();
void GetNextBlock();
void WriteGlass(int x, int y);
void WriteStatic();
void CheckForDelete();

bool isAvailableMove(int curX, int curY, int type, int rot, int moveX, int moveY);
bool isAvailableRot(int curX, int curY, int type, int rot, int dest);

//threads
void Input();
void Fall();
void GameOver();
void startServer();

[[noreturn]] void Multiplayer();


bool isGameOver = false;
int glass[HEIGHT][WIDTH];
int glassStatic[HEIGHT][WIDTH];
int nextBlock[2]={7,7};
int score[1];
int currentX = 5;
int currentY = 0;
int timer = 600;



int main() {
    srandom((unsigned) time(nullptr));
    Create();
    GetNextBlock();
    WriteGlass(currentX, currentY);
    int x = 0;
    std::cout << "Choose game mode: \n Type 1 for multiplayer or 2 for single-player \n";
    std::cin >> x;
    if(x==1)
        isMultiplayer = true;
    else
        isMultiplayer = false;
   if (isMultiplayer){
       std::cout << "Connecting...";
       startServer();
   }

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
                glassStatic[i][j]=999;
            } else glassStatic[i][j]=0;
        }
    }
}

void Output() {
    std::system("clear");
    for (int i = GAP; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (glass[i][j] == 0) {
                std::cout << EMPTY BACKGROUND " ■";
            } else if (glass[i][j] == 1) {
                std::cout << BLOCK BACKGROUND " ■";
            } else if (glass[i][j] == 999) {
                std::cout << WALL BACKGROUND " ■";
            } else if (glass[i][j] == 2) {
                std::cout << A_BLOCK BACKGROUND " ■";
            } else if (glass[i][j] == 3 || glass[i][j] == 4) {
                std::cout << B_BLOCK BACKGROUND " ■";
            } else if (glass[i][j] == 5 || glass[i][j] == 6) {
                std::cout << C_BLOCK BACKGROUND " ■";
            } else if (glass[i][j] == 7) {
                std::cout << D_BLOCK BACKGROUND " ■";
            }

        }
        std::cout << BACKGROUND " " CLEAN << std::endl;
    }
    std::cout << std::endl << "Your Score: " << score[0] << std::endl << std::endl;
    if (isMultiplayer) {
        std::cout << "Enemy Score: " << enScore[0] << std::endl << std::endl;
        for (int i = GAP; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                if (i==21 || j<=1 || j >=12){
                    std::cout << WALL BACKGROUND " ■";
                }else if (buffer[i][j] == 0) {
                    std::cout << EMPTY BACKGROUND " ■";
                } else if (buffer[i][j] == 9) {
                    std::cout << A_BLOCK BACKGROUND " ■";
                } else {
                    std::cout << BLOCK BACKGROUND " ■";
                }
            }
            std::cout << "\n";
            if (isGameOver) {
                GameOver();
            }
        }

    }
    std::cout << BACKGROUND " " CLEAN << std::endl;
}

void GetNextBlock(){
    int type = (int) random() % 7;
    int rot = (int) random() % 4;
    nextBlock[0] = type;
    nextBlock[1] = rot;
}

bool isAvailableMove(int curX, int curY, int type, int rot, int moveX, int moveY){
    int counter=0;
    for(int y=0; y<5; y++){
        for(int x=0; x<5; x++){
            if((glassStatic[y+curY+moveY][x+curX+moveX]+pTetra.GetBlock(type,rot,y,x)>pTetra.GetBlock(type,rot,y,x)&&pTetra.GetBlock(type,rot,y,x)!=0)||glassStatic[y+curY+moveY][x+curX+moveX]+pTetra.GetBlock(type,rot,y,x)>999){
                counter++;
            }
        }
    }
    if(counter>0)
        return false;
    else
        return true;
}

bool isAvailableRot(int curX, int curY, int type, int rot, int dest){
    int counter=0;
    for(int y=0; y<5; y++){
        for(int x=0; x<5; x++){
            if(glassStatic[curY+y][curX+x]+pTetra.GetBlock(type,rot+dest,y,x)!=pTetra.GetBlock(type,rot+dest,y,x)&&glassStatic[curY+y][curX+x]+pTetra.GetBlock(type,rot+dest,y,x)!=999){
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
    if(score[0] > 50000)
    	timer = 20;
    else if(score[0] > 30000)
        timer = 50;
    else if(score[0] > 20000)
        timer = 100;
    else if(score[0] > 10000)
        timer = 200;
    else if(score[0] > 8000)
        timer = 300;
    else if (score[0] >5000)
        timer = 400;
    else if (score[0] > 3000)
        timer = 500;
    else
        timer = 600;

    if(glassStatic[2][5]!=0||glassStatic[2][6]!=0||glassStatic[2][7]!=0){
        isGameOver = true;
        std::cout << CLEAN << std::endl;
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
        score[0]+=40;
    else if(linesCounter==2)
        score[0]+=100;
    else if(linesCounter==3)
        score[0]+=300;
    else if(linesCounter==4)
        score[0]+=1000;
}

void Input() {
    while (!isGameOver) {
        switch (mIO.GetKey()) {
            case 1: //Up
                if (nextBlock[1] == 3) {
                    if (isAvailableRot(currentX, currentY, nextBlock[0], nextBlock[1], -3))
                        nextBlock[1] = 0;
                } else if (isAvailableRot(currentX, currentY, nextBlock[0], nextBlock[1], 1))
                    nextBlock[1]++;
                WriteGlass(currentX, currentY);
                Output();
                break;
            case 2: //Down
                if (isAvailableMove(currentX, currentY, nextBlock[0], nextBlock[1], 0, 1)) {
                    currentY++;
                    WriteGlass(currentX, currentY);
                }
                Output();
                break;
            case 3: //Right
                if (isAvailableMove(currentX, currentY, nextBlock[0], nextBlock[1], 1, 0)) {
                    currentX++;
                    WriteGlass(currentX, currentY);
                }
                Output();
                break;
            case 4: //Left
                if (isAvailableMove(currentX, currentY, nextBlock[0], nextBlock[1], -1, 0)) {
                    currentX--;
                    WriteGlass(currentX, currentY);
                }
                Output();
                break;
            default:
                continue;
        }
    }
}
void Fall(){
    while (!isGameOver) {
        std::this_thread::sleep_for(std::chrono::milliseconds(timer));
        if (!isAvailableMove(currentX, currentY, nextBlock[0], nextBlock[1], 0, 1)) {
            WriteStatic();
            GetNextBlock();
            currentX = 5;
            currentY = 0;
            WriteGlass(currentX, currentY);
        } else {
            currentY++;
            WriteGlass(currentX, currentY);
        }
        Output();
    }
}

void GameOver() {
    while (true) {
        if (isGameOver) {
            std::system("clear");
            std::cout << CLEAN;
            if(isMultiplayer)
                std::cout << "\nGame over \nYour score: " << score[0] << "\n" << "Enemy score: " << enScore[0] << "\n";
            else
                std::cout << "\nGame over \nYour score: " << score[0];
            if (enemy_game_over[0] == 1) {
                // closing the connected socket
                close(new_socket);
                // closing the listening socket
                shutdown(server_fd, SHUT_RDWR);
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void startServer()
{
    struct sockaddr_in address{};
    int opt = 1;
    int addressLength = sizeof(address);
        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        // Forcefully attaching socket to the port 8080
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            perror("port attachment failed");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        // Forcefully attaching socket to the port 8080
        if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 3) < 0) {
            perror("s"); //printf("%s\n", buffer);r("listen");
            exit(EXIT_FAILURE);
        }
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addressLength)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

    read(new_socket, buffer, 1232);
    send(new_socket, glass, 1232, 0);
    std::thread t4(Multiplayer);
    Output();
    std::thread t1(Input);
    std::thread t2(Fall);
    std::thread t3(GameOver);
    t4.join();
    t2.join();
    t1.join();
    t3.join();

}

[[noreturn]] void Multiplayer(){
    while(true){
        if(isGameOver)
            game_over[0] = 1;

        read(new_socket, buffer, 1232);
        send(new_socket, glass, 1232, 0);
        read(new_socket, enScore, 4);
        send(new_socket, score, 4, 0);
        read(new_socket, enemy_game_over, 4);
        send(new_socket, game_over, 4, 0);
        if(game_over[0]==1 && enemy_game_over[0]==1){
            // closing the connected socket
            close(new_socket);
            // closing the listening socket
            shutdown(server_fd, SHUT_RDWR);
            GameOver();
        }
    }
}