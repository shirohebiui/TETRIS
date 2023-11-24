#include <stdio.h>
#include <windows.h>

//게임판 x,y좌표크기 지정

#define SCREEN_X 11 //게임화면 가로크기
#define SCREEN_Y 23 //게임화면 세로크기
#define SCREEN_X_ADJ 3 //게임화면 위치조정
#define SCREEN_Y_ADJ 1 //게임화면 위치조정

#define STATUS_X_ADJ SCREEN_X_ADJ+SCREEN_X+1 //게임정보표시 위치조정

void gotoxy(int x, int y) { //gotoxy함수
    COORD pos = { 2 * x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void setcursortype() { //커서숨기는 함수
    CONSOLE_CURSOR_INFO CurInfo;
    CurInfo.dwSize = 1;
    CurInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

//게임 전체 인터페이스를 표시
void draw_map() {          //게임 상태 표시를 나타내는 함수
    int y = 3;             // level, goal, score만 게임중에 값이 바뀔수 도 있음 그 y값을 따로 저장해둠
    // 그래서 혹시 게임 상태 표시 위치가 바뀌어도 그 함수에서 안바꿔도 되게..
    gotoxy(STATUS_X_ADJ, y + 2); printf("* * N E X T * * ");
    gotoxy(STATUS_X_ADJ, y + 3); printf("*             * ");
    gotoxy(STATUS_X_ADJ, y + 4); printf("*             * ");
    gotoxy(STATUS_X_ADJ, y + 5); printf("*             * ");
    gotoxy(STATUS_X_ADJ, y + 6); printf("*             * ");
    gotoxy(STATUS_X_ADJ, y + 7); printf("* * * * * * * * ");
    gotoxy(STATUS_X_ADJ, y + 9); printf("  △   : Shift        SPACE : Hard Drop");
    gotoxy(STATUS_X_ADJ, y + 10); printf("◁  ▷ : Left / Right   P   : Pause");
    gotoxy(STATUS_X_ADJ, y + 11); printf("  ▽   : Soft Drop     ESC  : Quit");
}