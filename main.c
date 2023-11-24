#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include "blocks.h"
#include "ScreenManage.h"
#include "RankingManage.h"
#include "key_input.h"

#define false 0
#define true 1

//***************** 전역 변수들 **************************************
int i,j;            //임시변수 (자주쓰므로 미리 할당)

int b_type;         //블록 종류를 저장 (7가지 중 한가지를 정할때 사용하는 변수)
int b_rotation;     //블록 회전값 저장 (4가지 회전값중 하나를 정할때 사용하는 변수)
int b_type_next;    //다음 블록값을 저장하는 변수

// 가장 많이 전수조사, 부분탐색이 시행되는것이 출력, 업데이트에 대한 공간이다.
// 따라서 공간에 저장하는값의 범위가 char의 범위내이므로 char로 선언하여 오버헤드를 줄이고자함
char SCREEN_UPDATE[SCREEN_Y][SCREEN_X]; //업데이트 내용이 담김
char SCREEN_MEMORY[SCREEN_Y][SCREEN_X]; //출력 화면

int bx, by; //이동중인 블록의 게임판상의 x,y좌표를 저장

int key; //키보드로 입력받은 키값을 저장

int speed; //게임진행속도 (Sleep함수의 인자값으로 사용예정)
int score; //현재 점수
int last_score = 0; //마지막게임점수
int best_score = 0; //최고게임점수

int new_block_on = 0; //새로운 블럭이 필요함을 알리는 flag
int crush_on = 0; //현재 이동중인 블록이 충돌상태인지 알려주는 flag
int space_key_on = 0; //hard drop상태임을 알려주는 flag (스페이스바를 누를 경우 -> hard drop 상태)

int PAUSE_FLAG = 0;

// ************************ function *******************************

void reset(void); //게임판 초기화
void reset_game(void); //메인 게임판(SCREEN_UPDATE[][]를 초기화)
void reset_game_cpy(void); //copy 게임판(SCREEN_MEMORY[][]를 초기화)
void make_new_block(void); //새로운 블록을 하나 만듦
void down_block(void); //블록을 아래로 떨어트림
int check_crush(int bx, int by, int rotation); //bx, by위치에 rotation회전값을 같는 경우 충돌 판단
void move_block(int dir); //dir방향으로 블록을 움직임
void init_game(); //
int GameIsValid(); //게임 오버 판별(쌓인블럭이 천장을 넘었는지 판별)

/***************************************************************************************
****************************************************************************************/
int main(void)
{

    srand(time(NULL));  //새로운 블록을 생성할때 랜덤으로 블록을 정하기 위해 rand함수 사용하기 위한 코드
    setcursortype();    //커서를 없애는 함수실행
    init_game();


    while (GameIsValid()) {
        draw_game(SCREEN_UPDATE, SCREEN_MEMORY);
        int ready_time = 10; // 입력을 받을 시간
        while(ready_time) { // 하강까지 대기하며 키입력에 따른 작동
            int key = key_scanf();
            switch(key) {
                case IsArrowKey: //방향키입력 -> 방향입력
                    move_block(key_scanf());
                case SPACE: //블럭 회전,이동
                    move_block(key);
                case ESC: //프로그램 종료
                    exit(1);
                case PAUSE: //일시정지
                    if(PAUSE_FLAG) system("PAUSE");
                    else           system("PAUSE");
                default:
                    1;
            }
            Sleep(speed/10);
            ready_time--;
        }
        down_block();
        if (new_block_on == 1) // drop_block()함수에서 바닥에 닿았을경우 새로운 블록을 부르기 위해 new_block_on을 1로 지정한다.
            make_new_block();
    }

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

void init_game()
{
    speed = 100;
    reset_game();       //초기 벽과 천장을 그려주는 함수 실행
    draw_game(SCREEN_UPDATE, SCREEN_MEMORY);        //블록들을 상태별로 그려주는 함수  (SCREEN_MEMORY[i][j] 와 SCREEN_UPDATE[i][j]를 비교해서 결정)
    draw_map();

    b_type_next = rand() % 7;
    make_new_block();        //새로운 블록을 게임맵 상단에 위치시키고 다음 블록을 생성해서 도움말창에 그려주는 함수
    draw_game(SCREEN_UPDATE, SCREEN_MEMORY);
}

void reset_game() {
    memset(SCREEN_UPDATE, 0, sizeof(SCREEN_UPDATE));
    memset(SCREEN_MEMORY, 100, sizeof(SCREEN_UPDATE));


    memset(&SCREEN_UPDATE[3][0], CEILLING, SCREEN_X); // 천장 설정

    //사이드 벽 설정
    for (i = 1; i < SCREEN_Y - 1; i++) {
        SCREEN_UPDATE[i][0] = WALL;
        SCREEN_UPDATE[i][SCREEN_X - 1] = WALL;
    }

    //바닥 벽 설정
    memset(&SCREEN_UPDATE[SCREEN_Y - 1][0], WALL, SCREEN_X);
}

void down_block() {
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



int check_crush(int bx, int by, int b_rotation) {
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (blocks[b_type][b_rotation][i][j] == 1 && SCREEN_UPDATE[by + i][bx + j] > 0) return false;
        }
    }
    return true;
};

void make_new_block(void) { //새로운 블록 생성
    bx = (SCREEN_X / 2) - 1;
    by = 0;
    b_type = b_type_next;
    b_type_next = rand() % 7;
    b_rotation = 0;

    new_block_on = 0;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j] = ACTIVE_BLOCK; //초기위치인 [bx,by]에서 updata에 저장된 블록 형태 출력
        }
    }
    for (i = 1; i < 3; i++) { //게임상태표시에 다음에 나올블럭을 그림
        for (j = 0; j < 4; j++) {
            if (blocks[b_type_next][0][i][j] == 1) {  //다음 그림의 형태는 b_type_next형의 첫번째 회전모양으로 함.
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

void move_block(int dir) {
    switch (dir) {
    case LEFT:
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
        bx--;
        break;

    case RIGHT:
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

    case DOWN:
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
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j] = EMPTY;
            }
        }
        b_rotation = (b_rotation + 1) % 4;
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j] = ACTIVE_BLOCK;
            }
        }
        break;

    case 100: // 바닥에 닿는 시점에 방향을 바꿀 수 있도록 추가
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

int GameIsValid() {
    for(i = 0; i < SCREEN_X; i++) {
        if(SCREEN_MEMORY[3][i] == INACTIVE_BLOCK) return false;
    }
    return true;
}