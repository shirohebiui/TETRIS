#include<stdio.h>
#include<windows.h>
#include<conio.h>
#include<time.h>
#include<stdlib.h>


#define LEFT 75 //좌로 이동    //키보드값들
#define RIGHT 77 //우로 이동
#define UP 72 //회전
#define DOWN 80 //soft drop
#define SPACE 32 //hard drop
#define p 112 //일시정지
#define P 80 //일시정지
#define ESC 27 //게임종료

#define false 0
#define true 1

#define ACTIVE_BLOCK -2 // 게임판배열에 저장될 블록의 상태들
#define CEILLING -1     // 블록이 이동할 수 있는 공간은 0 또는 음의 정수료 표현
#define EMPTY 0         // 블록이 이동할 수 없는 공간은 양수로 표현
#define WALL 1
#define INACTIVE_BLOCK 2 // 이동이 완료된 블록값

#define MAIN_X 11 //게임판 가로크기
#define MAIN_Y 23 //게임판 세로크기
#define MAIN_X_ADJ 3 //게임판 위치조정
#define MAIN_Y_ADJ 1 //게임판 위치조정

#define STATUS_X_ADJ MAIN_X_ADJ+MAIN_X+1 //게임정보표시 위치조정

void gotoxy(int x, int y) { //gotoxy함수
    COORD pos = { 2 * x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// 기본구성 : 세로20 가로10
// + 맵을 감싸는 외부벽 1칸 비커형
#define SCREEN_H 21
#define SCREEN_W 12
char SCREEN_MEMORY[SCREEN_H][SCREEN_H];
char SCREEN_UPDATE[SCREEN_W][SCREEN_W];

void init_screen() {
    for(int i=0; i<SCREEN_H; i++) {
        SCREEN_MEMORY[i][0] = WALL;
        SCREEN_UPDATE[i][0] = WALL;
        SCREEN_MEMORY[i][SCREEN_W-1] = WALL;
        SCREEN_UPDATE[i][SCREEN_W-1] = WALL;
    }
    memset(&SCREEN_MEMORY[SCREEN_H-1][0], WALL, SCREEN_W);
    memset(&SCREEN_UPDATE[SCREEN_H-1][0], WALL, SCREEN_W);
}


void draw_main(void) { //게임판 그리는 함수
    int i, j;
    // for (j = 1; j < MAIN_X - 1; j++) { //천장은 계속 새로운블럭이 지나가서 지워지면 새로 그려줌
    //     if (SCREEN_MEMORY[3][j] == EMPTY) SCREEN_MEMORY[3][j] = CEILLING;
    // }
    for (i = 0; i < SCREEN_H; i++) {
        for (j = 0; j < SCREEN_W; j++) {
            //if (SCREEN_MEMORY[i][j] != SCREEN_UPDATE[i][j]) { //cpy랑 비교해서 값이 달라진 부분만 새로 그려줌.
                //이게 없으면 게임판전체를 계속 그려서 느려지고 반짝거림
                gotoxy(j,i);
                switch (SCREEN_UPDATE[i][j]) {
                case EMPTY: //빈칸모양
                    printf("  ");
                    break;
                case CEILLING: //천장모양
                    printf(". ");
                    break;
                case WALL: //벽모양
                    printf("▩");
                    break;
                case INACTIVE_BLOCK: //굳은 블럭 모양
                    printf("□");
                    break;
                case ACTIVE_BLOCK: //움직이고있는 블럭 모양
                    printf("■");
                    break;
                }
            }
        //}
    }
    memmove(SCREEN_MEMORY, SCREEN_UPDATE, sizeof(SCREEN_MEMORY));
}
int main() {
    init_screen();
    while(1) {
    draw_main();
        Sleep(100);
    }
    return 0;
}