#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include "blocks.h"

#define false 0
#define true 1

// 키보드 입력값 지정

#define LEFT 75 //좌
#define RIGHT 77 //우
#define DOWN 80 //soft drop
#define UP 72 //회전
#define SPACE 32 //hard drop
#define p 112 //일시정지
#define P 80 //일시정지
#define ESC 27 //게임종료

//게임판 x,y좌표크기 지정

#define MAIN_X 11 //게임판 가로크기
#define MAIN_Y 23 //게임판 세로크기
#define MAIN_X_ADJ 3 //게임판 위치조정
#define MAIN_Y_ADJ 1 //게임판 위치조정

#define STATUS_X_ADJ MAIN_X_ADJ+MAIN_X+1 //게임정보표시 위치조정

//게임창 블록들의 상태 지정

#define ACTIVE_BLOCK -2 // 게임판배열에 저장될 블록의 상태들
#define CEILLING -1     // 블록이 이동할 수 있는 공간은 0 또는 음의 정수료 표현
#define EMPTY 0         // 블록이 이동할 수 없는 공간은 양수로 표현
#define WALL 1
#define INACTIVE_BLOCK 2 // 이동이 완료된 블록값



//***************** 전역 변수들 **************************************

int b_type; //블록 종류를 저장
int b_rotation; //블록 회전값 저장
int b_type_next; //다음 블록값 저장

int main_org[MAIN_Y][MAIN_X];
int main_cpy[MAIN_Y][MAIN_X];

int bx, by; //이동중인 블록의 게임판상의 x,y좌표를 저장

int key; //키보드로 입력받은 키값을 저장

int speed; //게임진행속도
int level; //현재 level
int level_goal; //다음레벨로 넘어가기 위한 목표점수
int cnt; //현재 레벨에서 제거한 줄 수를 저장
int score; //현재 점수
int last_score = 0; //마지막게임점수
int best_score = 0; //최고게임점수

int new_block_on = 0; //새로운 블럭이 필요함을 알리는 flag
int crush_on = 0; //현재 이동중인 블록이 충돌상태인지 알려주는 flag
int level_up_on = 0; //다음레벨로 진행(현재 레벨목표가 완료되었음을) 알리는 flag
int space_key_on = 0; //hard drop상태임을 알려주는 flag

// ************************ function *******************************

void title(void); //게임시작화면
void reset(void); //게임판 초기화
void reset_main(void); //메인 게임판(main_org[][]를 초기화)
void reset_main_cpy(void); //copy 게임판(main_cpy[][]를 초기화)
void draw_map(void); //게임 전체 인터페이스를 표시
void draw_main(void); //게임판을 그림
void new_block(void); //새로운 블록을 하나 만듦
void check_key(void); //키보드로 키를 입력받음
void drop_block(void); //블록을 아래로 떨어트림
int check_crush(int bx, int by, int rotation); //bx, by위치에 rotation회전값을 같는 경우 충돌 판단
void move_block(int dir); //dir방향으로 블록을 움직임
void check_line(void); //줄이 가득찼는지를 판단하고 지움
void check_level_up(void); //레벨목표가 달성되었는지를 판단하고 levelup시킴
void check_game_over(void); //게임오버인지 판단하고 게임오버를 진행
void pause(void);//게임을 일시정지시킴

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

int main(void)
{
    int i = 0;
    speed = 100;
    while(1) {
        srand(time(NULL));  //새로운 블록을 생성할때 랜덤으로 블록을 정하기 위해 rand함수 사용하기 위한 코드
        setcursortype();    //커서를 없애는 함수실행
        reset_main();       //초기 벽과 천장을 그려주는 함수 실행
        draw_map();         //게임을 도와주는 도움말을 옆에 그려주는 함수
        draw_main();        //블록들을 상태별로 그려주는 함수  (main_cpy[i][j] 와 main_org[i][j]를 비교해서 결정)


        b_type_next = rand() % 7;
        new_block();        //새로운 블록을 게임맵 상단에 위치시키고 다음 블록을 생성해서 도움말창에 그려주는 함수

        while (crush_on == 0) { //바닥에 닿을때까지 이동시킴
            draw_main();   //새로운 맵을 계속 그려준
            drop_block();  //조건들을 따지면서 main_org를 새로 지정해준다. (충돌조건 만족시 y축방향으로 1칸씩 이동)
            Sleep(100);    //C언어 지연함수 windows.h선언하면 사용가능 (100 -> 0.1초)
        }
    }
    return 0;
}


void draw_map(void) { //게임 상태 표시를 나타내는 함수
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

void draw_main(void) { //게임판 그리는 함수
    int i, j;

    for (j = 1; j < MAIN_X - 1; j++) { //천장은 계속 새로운블럭이 지나가서 지워지면 새로 그려줌
        if (main_org[3][j] == EMPTY) main_org[3][j] = CEILLING;
    }
    for (i = 0; i < MAIN_Y; i++) {
        for (j = 0; j < MAIN_X; j++) {
            if (main_cpy[i][j] != main_org[i][j]) { //cpy랑 비교해서 값이 달라진 부분만 새로 그려줌.
                //이게 없으면 게임판전체를 계속 그려서 느려지고 반짝거림
                gotoxy(MAIN_X_ADJ + j, MAIN_Y_ADJ + i);
                switch (main_org[i][j]) {
                case EMPTY: //빈칸모양
                    printf("  ");
                    break;
                case CEILLING: //천장모양
                    printf("- ");
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
        }
    }
    for (i = 0; i < MAIN_Y; i++) { //게임판을 그린 후 main_cpy에 복사
        for (j = 0; j < MAIN_X; j++) {
            main_cpy[i][j] = main_org[i][j];
        }
    }
}

void reset_main(void) { //게임판을 초기화
    int i, j;

    for (i = 0; i < MAIN_Y; i++) { // 게임판을 0으로 초기화
        for (j = 0; j < MAIN_X; j++) {
            main_org[i][j] = 0;
            main_cpy[i][j] = 100;
        }
    }
    for (j = 1; j < MAIN_X; j++) { //y값이 3인 위치에 천장을 만듦
        main_org[3][j] = CEILLING;
    }
    for (i = 1; i < MAIN_Y - 1; i++) { //좌우 벽을 만듦
        main_org[i][0] = WALL;
        main_org[i][MAIN_X - 1] = WALL;
    }
    for (j = 0; j < MAIN_X; j++) { //바닥벽을 만듦
        main_org[MAIN_Y - 1][j] = WALL;
    }
}

void drop_block(void) {
    int i, j;

    if (crush_on && check_crush(bx, by + 1, b_rotation) == true) crush_on = 0; //밑이 비어있으면 crush flag 끔
    if (crush_on && check_crush(bx, by + 1, b_rotation) == false) { //밑이 비어있지않고 crush flag가 켜저있으면
        for (i = 0; i < MAIN_Y; i++) { //현재 조작중인 블럭을 굳힘
            for (j = 0; j < MAIN_X; j++) {
                if (main_org[i][j] == ACTIVE_BLOCK) main_org[i][j] = INACTIVE_BLOCK;
            }
        }
        crush_on = 0; //flag를 끔
        check_line(); //라인체크를 함
        new_block_on = 1; //새로운 블럭생성 flag를 켬
        return; //함수 종료
    }
    if (check_crush(bx, by + 1, b_rotation) == true) move_block(DOWN); //밑이 비어있으면 밑으로 한칸 이동
    if (check_crush(bx, by + 1, b_rotation) == false) crush_on++; //밑으로 이동이 안되면  crush flag를 켬
}

int check_crush(int bx, int by, int b_rotation) { //지정된 좌표와 회전값으로 충돌이 있는지 검사
    int i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) { //지정된 위치의 게임판과 블럭모양을 비교해서 겹치면 false를 리턴
            if (blocks[b_type][b_rotation][i][j] == 1 && main_org[by + i][bx + j] > 0) return false;
        }
    }
    return true; //하나도 안겹치면 true리턴
};

void move_block(int dir) { //블록을 이동시킴
    int i, j;

    switch (dir) {
    case LEFT: //왼쪽방향
        for (i = 0; i < 4; i++) { //현재좌표의 블럭을 지움
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
            }
        }
        for (i = 0; i < 4; i++) { //왼쪽으로 한칸가서 active block을 찍음
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j - 1] = ACTIVE_BLOCK;
            }
        }
        bx--; //좌표값 이동
        break;

    case RIGHT:    //오른쪽 방향. 왼쪽방향이랑 같은 원리로 동작
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
            }
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j + 1] = ACTIVE_BLOCK;
            }
        }
        bx++;
        break;

    case DOWN:    //아래쪽 방향. 왼쪽방향이랑 같은 원리로 동작
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
            }
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i + 1][bx + j] = ACTIVE_BLOCK;
            }
        }
        by++;
        break;

    case UP: //키보드 위쪽 눌렀을때 회전시킴.
        for (i = 0; i < 4; i++) { //현재좌표의 블럭을 지움
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
            }
        }
        b_rotation = (b_rotation + 1) % 4; //회전값을 1증가시킴(3에서 4가 되는 경우는 0으로 되돌림)
        for (i = 0; i < 4; i++) { //회전된 블록을 찍음
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = ACTIVE_BLOCK;
            }
        }
        break;

    case 100: //블록이 바닥, 혹은 다른 블록과 닿은 상태에서 한칸위로 올려 회전이 가능한 경우
        //이를 동작시키는 특수동작
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
            }
        }
        b_rotation = (b_rotation + 1) % 4;
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i - 1][bx + j] = ACTIVE_BLOCK;
            }
        }
        by--;
        break;
    }
}

void check_line(void) {
    int i, j, k, l;

    int block_amount; //한줄의 블록갯수를 저장하는 변수
    int combo = 0; //콤보갯수 저장하는 변수 지정및 초기화

    for (i = MAIN_Y - 2; i > 3;) { //i=MAIN_Y-2 : 밑쪽벽의 윗칸부터,  i>3 : 천장(3)아래까지 검사
        block_amount = 0; //블록갯수 저장 변수 초기화
        for (j = 1; j < MAIN_X - 1; j++) { //벽과 벽사이의 블록갯루를 셈
            if (main_org[i][j] > 0) block_amount++;
        }
        if (block_amount == MAIN_X - 2) { //블록이 가득 찬 경우
            if (level_up_on == 0) { //레벨업상태가 아닌 경우에(레벨업이 되면 자동 줄삭제가 있음)
                score += 100 * level; //점수추가
                cnt++; //지운 줄 갯수 카운트 증가
                combo++; //콤보수 증가
            }
            for (k = i; k > 1; k--) { //윗줄을 한칸씩 모두 내림(윗줄이 천장이 아닌 경우에만)
                for (l = 1; l < MAIN_X - 1; l++) {
                    if (main_org[k - 1][l] != CEILLING) main_org[k][l] = main_org[k - 1][l];
                    if (main_org[k - 1][l] == CEILLING) main_org[k][l] = EMPTY;
                    //윗줄이 천장인 경우에는 천장을 한칸 내리면 안되니까 빈칸을 넣음
                }
            }
        }
        else i--;
    }
    if (combo) { //줄 삭제가 있는 경우 점수와 레벨 목표를 새로 표시함
        if (combo > 1) { //2콤보이상인 경우 경우 보너스및 메세지를 게임판에 띄웠다가 지움
            gotoxy(MAIN_X_ADJ + (MAIN_X / 2) - 1, MAIN_Y_ADJ + by - 2); printf("%d COMBO!", combo);
            Sleep(500);
            score += (combo * level * 100);

            //(main_cpy와 main_org가 전부 다르므로 다음번 draw()호출시 게임판 전체를 새로 그리게 됨)
        }

    }
}
void new_block(void) { //새로운 블록 생성
    int i, j;

    bx = (MAIN_X / 2) - 1; //블록 생성 위치x좌표(게임판의 가운데)
    by = 0;  //블록 생성위치 y좌표(제일 위)
    b_type = b_type_next; //다음블럭값을 가져옴
    b_type_next = rand() % 7; //다음 블럭을 만듦
    b_rotation = 0;  //회전은 0번으로 가져옴

    new_block_on = 0; //new_block flag를 끔

    for (i = 0; i < 4; i++) { //게임판 bx, by위치에 블럭생성
        for (j = 0; j < 4; j++) {
            if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = ACTIVE_BLOCK;
        }
    }
    for (i = 1; i < 3; i++) { //게임상태표시에 다음에 나올블럭을 그림
        for (j = 0; j < 4; j++) {
            if (blocks[b_type_next][0][i][j] == 1) {
                gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
                printf("■");
            }
            else {
                gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
                printf("  ");
            }
        }
    }
}

