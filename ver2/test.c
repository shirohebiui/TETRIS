// 키보드 입력 방법
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>

#define LEFT 75 //->버튼 - 좌로이동
#define RIGHT 77 //<-버튼 - 우로이동
#define DOWN 80 //下버튼 - 하강
#define UP 72 //上버튼 - 회전
#define SPACE 32 //space bar - 바닥까지 하강
#define ESC 27 //esc - 프로그램 종료
#define PAUSE 112


//게임판 x,y좌표크기 지정

#define SCREEN_X 11 //게임화면 가로크기
#define SCREEN_Y 23 //게임화면 세로크기
#define SCREEN_X_ADJ 3 //게임화면 위치조정
#define SCREEN_Y_ADJ 1 //게임화면 위치조정

#define STATUS_X_ADJ SCREEN_X_ADJ+SCREEN_X+1 //게임정보표시 위치조정
//게임창 블록들의 상태

#define ACTIVE_BLOCK -2     // 게임판배열에 저장될 블록의 상태들
#define CEILLING -1         // 블록이 이동할 수 있는 공간은 0 또는 음의 정수료 표현
#define EMPTY 0             // 블록이 이동할 수 없는 공간은 양수로 표현
#define WALL 1              // 벽 (이동할 수 없는 블록)
#define INACTIVE_BLOCK 2    // 이동이 완료된 블록값 (고정된 블록)

void test_map(char map[][SCREEN_X]) {
    for(int i=0; i<SCREEN_Y; i++) {
        printf("Y%2d ", i);
        for(int j=0; j<SCREEN_X; j++) {
            printf("%d", map[i][j]);
        }
        printf("\n");
    }
}
int clear_line(char SCREEN_UPDATE[][SCREEN_X], int by) {
    int line = 0;
    int top = SCREEN_Y;
    for (int i = 0; i < 4; i++) {
        int sum = 0;
        for(int j = 0; j < SCREEN_X; j++) {
            if(SCREEN_UPDATE[by+i][j] > 0) sum++;
        }
        if(sum == SCREEN_X) {
            line++;
            memmove(&SCREEN_UPDATE[5], &SCREEN_UPDATE[4], (by+i-4) * SCREEN_X);
            memset(&SCREEN_UPDATE[4][1], EMPTY, SCREEN_X-2);
        }
    }
    return line+1;
}


int main() {
    char SCREEN[SCREEN_Y][SCREEN_X] = {0, };
    memset(&SCREEN[4], 1, (SCREEN_Y-4)*SCREEN_X);
    memset(&SCREEN[SCREEN_Y-4][1], 0, 1);
    clear_line(SCREEN, SCREEN_Y-4);
    test_map(SCREEN);
    while(1) {}
    return 0;
}