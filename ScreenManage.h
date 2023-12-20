#include <stdio.h>
#include <string.h>
#include <windows.h>

//게임판 x,y좌표크기 지정

#define SCREEN_X 11 //게임화면 가로크기
#define SCREEN_Y 23 //게임화면 세로크기
#define SCREEN_X_ADJ 3 //게임화면 위치조정
#define SCREEN_Y_ADJ 1 //게임화면 위치조정

#define STATUS_X_ADJ SCREEN_X_ADJ+SCREEN_X+1 //게임정보표시 위치조정

void test_map(char map[][SCREEN_X]);
int sum_line(char map[][SCREEN_X], int y);

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



//게임창 블록들의 상태

#define ACTIVE_BLOCK -2     // 게임판배열에 저장될 블록의 상태들
#define CEILLING -1         // 블록이 이동할 수 있는 공간은 0 또는 음의 정수료 표현
#define EMPTY 0             // 블록이 이동할 수 없는 공간은 양수로 표현
#define WALL 1              // 벽 (이동할 수 없는 블록)
#define INACTIVE_BLOCK 2    // 이동이 완료된 블록값 (고정된 블록)

void title(void) {
    int x=0, y=0;

    system("cls");
    gotoxy(x + 20, y + 4); printf("   2023 자료구조 기말 과제   ");
    gotoxy(x + 20, y + 6); printf("   \"테트리스 게임\"        ");

    gotoxy(x + 20, y + 9); printf("   <Menu>   ");
    gotoxy(x + 20, y + 10); printf("   1. 게임 시작  ");
    gotoxy(x + 20, y + 11); printf("   2. 랭킹        ");
    gotoxy(x + 20, y + 12); printf("   3. 프로그램 종료   ");
    gotoxy(x + 25, y + 15); printf("   강민수(1872301)       ");
    gotoxy(x + 25, y + 16); printf("   김재현(2071333)   ");

    while (1) {
        if (kbhit()) {
            int key = key_scanf();

            if (key == '1') {
                break;
            } else if (key == '2') {
                system("cls");
                only_ranking_list();
                printf("메인으로 돌아가기(y or Y)");
                fflush(stdout);
                key = key_scanf();
                 while (1) {
                    if (kbhit()) {
                        key = key_scanf();
                        if (key == 'y' || key == 'Y') {
                            system("cls");
                            title(); // Go back to the main menu
                            break;
                        }
                    }
                }
            } else if (key == '3') {
                system("cls");
                printf("프로그램 종료");
                exit(1);
            }
        }
    }

    while (kbhit()) getch(); // Clear input buffer
}

void draw_game(char SCREEN_UPDATE[][SCREEN_X], char SCREEN_MEMORY[][SCREEN_X]) { //게임판 그리는 함수
    int i, j;
    for (j = 1; j < SCREEN_X - 1; j++) { //천장은 계속 새로운블럭이 지나가서 지워지면 새로 그려줌
        if (SCREEN_UPDATE[3][j] == EMPTY) SCREEN_UPDATE[3][j] = CEILLING;
    }
    for (i = 0; i < SCREEN_Y; i++) {
        for (j = 0; j < SCREEN_X; j++) {
            if (SCREEN_MEMORY[i][j] != SCREEN_UPDATE[i][j]) { //UPDATE 내용과 비교해서 차이가 있는부분만 화면갱신
                gotoxy(SCREEN_X_ADJ + j, SCREEN_Y_ADJ + i);
                switch (SCREEN_UPDATE[i][j]) {
                case EMPTY: //빈칸모양
                    printf("  ");
                    break;
                case CEILLING: //천장모양
                    printf("- ");
                    break;
                case WALL: //벽모양
                    printf("▩");
                    break;
                case INACTIVE_BLOCK: //고정된 블럭 모양
                    printf("□");
                    break;
                case ACTIVE_BLOCK: //움직이고있는 블럭 모양
                    printf("■");
                    break;
                }
            }
        }
    }

    memmove(SCREEN_MEMORY, SCREEN_UPDATE, SCREEN_Y*SCREEN_X); //게임화면을 갱신 (mem <- update)
}

void clear_line(char map[][SCREEN_X], int y) {
    memcpy(map[5], map[4], SCREEN_X * (y-4));
    memset(&map[4][1], EMPTY, SCREEN_X-2);
}

void test_map(char map[][SCREEN_X]) {
    int i,j;
    printf("test\n");
    for(i=0; i<SCREEN_Y; i++) {
        for(j=0; j<SCREEN_X; j++) {
            printf("%d", map[i][j]);
        }
        printf("\n");
    }
}


int sum_line(char map[][SCREEN_X], int y) {
    int i;
    if(y >= SCREEN_Y-1) return -1;
    int sum = 0;
    for(i=0; i<SCREEN_X; i++) {
        if(map[y][i]>0) sum++;
    }
    return sum;
}
