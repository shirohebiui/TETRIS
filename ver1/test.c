#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include "blocks.h"
#include "ScreenManage.h"
#include "RankingManage.h"

#define false 0
#define true 1

// 키보드 입력값 지정

#define LEFT 75 //->버튼 - 좌로이동
#define RIGHT 77 //<-버튼 - 우로이동
#define DOWN 80 //下버튼 - 하강
#define UP 72 //上버튼 - 회전
#define SPACE 32 //space bar - 바닥까지 하강
#define ESC 27 //esc - 프로그램 종료

//게임창 블록들의 상태 지정 !!

#define ACTIVE_BLOCK -2     // 게임판배열에 저장될 블록의 상태들
#define CEILLING -1         // 블록이 이동할 수 있는 공간은 0 또는 음의 정수료 표현
#define EMPTY 0             // 블록이 이동할 수 없는 공간은 양수로 표현
#define WALL 1              // 벽
#define INACTIVE_BLOCK 2    // 이동이 완료된 블록값


//***************** 전역 변수들 **************************************
int i,j;            //임시변수 (자주쓰므로 미리 할당)

int b_type;         //블록 종류를 저장
int b_rotation;     //블록 회전값 저장
int b_type_next;    //다음 블록값 저장

// 가장 많이 전수조사, 부분탐색이 시행되는것이 출력, 업데이트에 대한 공간이다.
// 따라서 공간에 저장하는값의 범위가 char의 범위내이므로 char로 선언하여 오버헤드를 줄이고자함
char SCREEN_UPDATE[SCREEN_Y][SCREEN_X]; //업데이트 내용이 담김
char SCREEN_MEMORY[SCREEN_Y][SCREEN_X]; //출력 화면

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

void reset(void); //게임판 초기화
void reset_main(void); //메인 게임판(SCREEN_UPDATE[][]를 초기화)
void reset_main_cpy(void); //copy 게임판(SCREEN_MEMORY[][]를 초기화)
void draw_main(void); //게임판을 그림
void new_block(void); //새로운 블록을 하나 만듦
void drop_block(void); //블록을 아래로 떨어트림
int check_crush(int bx, int by, int rotation); //bx, by위치에 rotation회전값을 같는 경우 충돌 판단
void move_block(int dir); //dir방향으로 블록을 움직임

/***************************************************************************************
****************************************************************************************/
int main(void)
{
    
    int i = 0;
    speed = 100;
    //while(1) {
        srand(time(NULL));  //새로운 블록을 생성할때 랜덤으로 블록을 정하기 위해 rand함수 사용하기 위한 코드
        setcursortype();    //커서를 없애는 함수실행
        reset_main();       //초기 벽과 천장을 그려주는 함수 실행
        draw_map();         //게임을 도와주는 도움말을 옆에 그려주는 함수
        draw_main();        //블록들을 상태별로 그려주는 함수  (SCREEN_MEMORY[i][j] 와 SCREEN_UPDATE[i][j]를 비교해서 결정)

        b_type_next = rand() % 7;
        new_block();        //새로운 블록을 게임맵 상단에 위치시키고 다음 블록을 생성해서 도움말창에 그려주는 함수

        while (crush_on == 0) { //바닥에 닿을때까지 이동시킴
            draw_main();   //새로운 맵을 계속 그려준
            drop_block();  //조건들을 따지면서 main_org를 새로 지정해준다. (충돌조건 만족시 y축방향으로 1칸씩 이동)
            Sleep(100);    //C언어 지연함수 windows.h선언하면 사용가능 (100 -> 0.1초)
        }
    //}
    gotoxy(20, 20);
    //system("cls");
    
    char id[ID_SIZE] = "USER1";     // ID는 유저입력에따라, SCORE는 게임결과에 따라 값을
    int score = 10000;              // 받을 예정이지만 아직 미구현이기에 임시로 임의의값을 저장함
    system("cls");
    init_DB();
    gotoxy(5, 5); recording_and_print_ranking(id, score);
    while(1) {
        //아직 종료함수 미구현
        //직접 프로그램끄기
        Sleep(500);
    }
    

    return 0;
}
/***************************************************************************************
****************************************************************************************/

void draw_main(void) { //게임판 그리는 함수
    for (j = 1; j < SCREEN_X - 1; j++) { //천장은 계속 새로운블럭이 지나가서 지워지면 새로 그려줌
        if (SCREEN_UPDATE[3][j] == EMPTY) SCREEN_UPDATE[3][j] = CEILLING;
    }
    for (i = 0; i < SCREEN_Y; i++) {
        for (j = 0; j < SCREEN_X; j++) {
            if (SCREEN_MEMORY[i][j] != SCREEN_UPDATE[i][j]) { //UPDATE 내용과 비교해서 차이가 있는부분만 화면갱신
                //이게 없으면 게임판전체를 계속 그려서 느려지고 반짝거림
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
    memmove(SCREEN_MEMORY, SCREEN_UPDATE, sizeof(SCREEN_UPDATE)); //게임화면을 갱신 (mem <- update)
}

void reset_main() { //게임판을 초기화
    // 게임판을 0으로 초기화
    memset(SCREEN_UPDATE, 0, sizeof(SCREEN_UPDATE));
    memset(SCREEN_MEMORY, 100, sizeof(SCREEN_UPDATE));
    
    //y값이 3인 위치에 천장을 만듦
    memset(&SCREEN_UPDATE[3][0], CEILLING, SCREEN_X);

    //옆벽
    for (i = 1; i < SCREEN_Y - 1; i++) {
        SCREEN_UPDATE[i][0] = WALL;
        SCREEN_UPDATE[i][SCREEN_X - 1] = WALL;
    }

    //바닥
    memset(&SCREEN_UPDATE[SCREEN_Y - 1][0], WALL, SCREEN_X);
}

void drop_block() {
    if (crush_on && check_crush(bx, by + 1, b_rotation) == true) crush_on = 0; //밑이 비어있으면 crush flag 끔
    if (crush_on && check_crush(bx, by + 1, b_rotation) == false) { //밑이 비어있지않고 crush flag가 켜저있으면
        for (i = 0; i < SCREEN_Y; i++) { //현재 조작중인 블럭을 굳힘
            for (j = 0; j < SCREEN_X; j++) {
                if (SCREEN_UPDATE[i][j] == ACTIVE_BLOCK) SCREEN_UPDATE[i][j] = INACTIVE_BLOCK;
            }
        }
        crush_on = 0; //flag를 끔
        //check_line(); //라인체크를 함
        new_block_on = 1; //새로운 블럭생성 flag를 켬
        return; //함수 종료
    }
    if (check_crush(bx, by + 1, b_rotation) == true) move_block(DOWN); //밑이 비어있으면 밑으로 한칸 이동
    if (check_crush(bx, by + 1, b_rotation) == false) crush_on++; //밑으로 이동이 안되면  crush flag를 켬
}

void move_block(int dir) { //블록을 이동시킴
    switch (dir) {
    case LEFT: //왼쪽방향
        for (i = 0; i < 4; i++) { //현재좌표의 블럭을 지움
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j] = EMPTY;
            }
        }
        for (i = 0; i < 4; i++) { //왼쪽으로 한칸가서 active block을 찍음
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j - 1] = ACTIVE_BLOCK;
            }
        }
        bx--; //좌표값 이동
        break;

    case RIGHT:    //오른쪽 방향. 왼쪽방향이랑 같은 원리로 동작
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j] = EMPTY;
            }
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j + 1] = ACTIVE_BLOCK;
            }
        }
        bx++;
        break;

    case DOWN:    //아래쪽 방향. 왼쪽방향이랑 같은 원리로 동작
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j] = EMPTY;
            }
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i + 1][bx + j] = ACTIVE_BLOCK;
            }
        }
        by++;
        break;

    case UP: //키보드 위쪽 눌렀을때 회전시킴.
        for (i = 0; i < 4; i++) { //현재좌표의 블럭을 지움
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j] = EMPTY;
            }
        }
        b_rotation = (b_rotation + 1) % 4; //회전값을 1증가시킴(3에서 4가 되는 경우는 0으로 되돌림)
        for (i = 0; i < 4; i++) { //회전된 블록을 찍음
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j] = ACTIVE_BLOCK;
            }
        }
        break;

    case 100: //블록이 바닥, 혹은 다른 블록과 닿은 상태에서 한칸위로 올려 회전이 가능한 경우
        //이를 동작시키는 특수동작
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j] = EMPTY;
            }
        }
        b_rotation = (b_rotation + 1) % 4;
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i - 1][bx + j] = ACTIVE_BLOCK;
            }
        }
        by--;
        break;
    }
}

int check_crush(int bx, int by, int b_rotation) { //지정된 좌표와 회전값으로 충돌이 있는지 검사
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) { //지정된 위치의 게임판과 블럭모양을 비교해서 겹치면 false를 리턴
            if (blocks[b_type][b_rotation][i][j] == 1 && SCREEN_UPDATE[by + i][bx + j] > 0) return false;
        }
    }
    return true; //하나도 안겹치면 true리턴
};

void new_block(void) { //새로운 블록 생성
    bx = (SCREEN_X / 2) - 1; //블록 생성 위치x좌표(게임판의 가운데)
    by = 0;  //블록 생성위치 y좌표(제일 위)
    b_type = b_type_next; //다음블럭값을 가져옴
    b_type_next = rand() % 7; //다음 블럭을 만듦
    b_rotation = 0;  //회전은 0번으로 가져옴

    new_block_on = 0; //new_block flag를 끔

    for (i = 0; i < 4; i++) { //게임판 bx, by위치에 블럭생성
        for (j = 0; j < 4; j++) {
            if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j] = ACTIVE_BLOCK;
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