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

//***************** ���� ������ **************************************
int i,j;            //�ӽú��� (���־��Ƿ� �̸� �Ҵ�)

int b_type;         //��� ������ ���� (7���� �� �Ѱ����� ���Ҷ� ����ϴ� ����)
int b_rotation;     //��� ȸ���� ���� (4���� ȸ������ �ϳ��� ���Ҷ� ����ϴ� ����)
int b_type_next;    //���� ��ϰ��� �����ϴ� ����

// ���� ���� ��������, �κ�Ž���� ����Ǵ°��� ���, ������Ʈ�� ���� �����̴�.
// ���� ������ �����ϴ°��� ������ char�� �������̹Ƿ� char�� �����Ͽ� ������带 ���̰�����
char SCREEN_UPDATE[SCREEN_Y][SCREEN_X]; //������Ʈ ������ ���
char SCREEN_MEMORY[SCREEN_Y][SCREEN_X]; //��� ȭ��

int bx, by; //�̵����� ����� �����ǻ��� x,y��ǥ�� ����

int key; //Ű����� �Է¹��� Ű���� ����

int speed; //��������ӵ� (Sleep�Լ��� ���ڰ����� ��뿹��)
int score; //���� ����
int last_score = 0; //��������������
int best_score = 0; //�ְ��������

int new_block_on = 0; //���ο� ���� �ʿ����� �˸��� flag
int crush_on = 0; //���� �̵����� ����� �浹�������� �˷��ִ� flag
int space_key_on = 0; //hard drop�������� �˷��ִ� flag (�����̽��ٸ� ���� ��� -> hard drop ����)

int PAUSE_FLAG = 0;

// ************************ function *******************************

void reset(void); //������ �ʱ�ȭ
void reset_game(void); //���� ������(SCREEN_UPDATE[][]�� �ʱ�ȭ)
void reset_game_cpy(void); //copy ������(SCREEN_MEMORY[][]�� �ʱ�ȭ)
void make_new_block(void); //���ο� ����� �ϳ� ����
void down_block(void); //����� �Ʒ��� ����Ʈ��
int check_crush(int bx, int by, int rotation); //bx, by��ġ�� rotationȸ������ ���� ��� �浹 �Ǵ�
int move_block(int dir); //dir�������� ����� ������, �����ϸ� 1 ����
void init_game(); //
int GameIsValid(); //���� ���� �Ǻ�(���κ��� õ���� �Ѿ����� �Ǻ�)
void user_cmd_scan(); //�����Է¹ޱ�
void block_clear();   //�� ���� ����
void block_update();  //�� ���� ������Ʈ
int count_score();

/***************************************************************************************
****************************************************************************************/
int main(void)
{

    srand(time(NULL));  //���ο� ����� �����Ҷ� �������� ����� ���ϱ� ���� rand�Լ� ����ϱ� ���� �ڵ�
    setcursortype();    //Ŀ���� ���ִ� �Լ�����
    init_game();

    int score = 0;
    draw_game(SCREEN_UPDATE, SCREEN_MEMORY);
    while (GameIsValid()) {
        int combo = 0;
        user_cmd_scan(speed/10);
        down_block();
        if (new_block_on == 1) { // drop_block()�Լ����� �ٴڿ� �������� ���ο� ����� �θ��� ���� new_block_on�� 1�� �����Ѵ�.
            make_new_block();
            combo = count_score();
            if(combo) {
                score += combo * 100; Sleep(100);
            }
        }
        draw_game(SCREEN_UPDATE, SCREEN_MEMORY);
    }

    gotoxy(20, 20);

    char id[ID_SIZE] = "USER1";     // ID�� �����Է¿�����, SCORE�� ���Ӱ���� ���� ����
    system("cls");
    printf("your score : %d\n", score);
    init_DB();
    gotoxy(5, 5); recording_and_print_ranking(id, score);
    system("Pause");
    Sleep(1000);

    return 0;
}
/***************************************************************************************
****************************************************************************************/

void init_game()
{
    speed = 100;
    reset_game();       //�ʱ� ���� õ���� �׷��ִ� �Լ� ����
    draw_game(SCREEN_UPDATE, SCREEN_MEMORY);        //��ϵ��� ���º��� �׷��ִ� �Լ�  (SCREEN_MEMORY[i][j] �� SCREEN_UPDATE[i][j]�� ���ؼ� ����)
    draw_map();

    b_type_next = rand() % 7;
    make_new_block();        //���ο� ����� ���Ӹ� ��ܿ� ��ġ��Ű�� ���� ����� �����ؼ� ����â�� �׷��ִ� �Լ�
    draw_game(SCREEN_UPDATE, SCREEN_MEMORY);
}

void reset_game() {
    memset(SCREEN_UPDATE, 0, sizeof(SCREEN_UPDATE));
    memset(SCREEN_MEMORY, 100, sizeof(SCREEN_UPDATE));


    memset(&SCREEN_UPDATE[3][0], CEILLING, SCREEN_X); // õ�� ����

    //���̵� �� ����
    for (i = 1; i < SCREEN_Y - 1; i++) {
        SCREEN_UPDATE[i][0] = WALL;
        SCREEN_UPDATE[i][SCREEN_X - 1] = WALL;
    }

    //�ٴ� �� ����
    memset(&SCREEN_UPDATE[SCREEN_Y - 1][0], WALL, SCREEN_X);
}

void down_block() {
    if (crush_on && check_crush(bx, by + 1, b_rotation) == true) crush_on = 0; //���� ��������� crush flag ��
    if (crush_on && check_crush(bx, by + 1, b_rotation) == false) { //���� ��������ʰ� crush flag�� ����������
        for (i = 0; i < SCREEN_Y; i++) { //���� �������� ���� ����
            for (j = 0; j < SCREEN_X; j++) {
                if (SCREEN_UPDATE[i][j] == ACTIVE_BLOCK) SCREEN_UPDATE[i][j] = INACTIVE_BLOCK;
            }
        }
        crush_on = 0; //flag�� ��
        //check_line(); //����üũ�� ��
        new_block_on = 1; //���ο� ������ flag�� ��
        return; //�Լ� ����
    }
    if (check_crush(bx, by + 1, b_rotation) == true) move_block(DOWN); //���� ��������� ������ ��ĭ �̵�
    if (check_crush(bx, by + 1, b_rotation) == false) crush_on++; //������ �̵��� �ȵǸ�  crush flag�� ��
}

int check_crush(int bx, int by, int b_rotation) {
    int ii, jj; //�Լ� ���� �����̹Ƿ� ���� �����Ҵ�
    for (ii = 0; ii < 4; ii++) {
        for (jj = 0; jj < 4; jj++) {
            if (blocks[b_type][b_rotation][ii][jj] == 1 && SCREEN_UPDATE[by + ii][bx + jj] > 0) return false;
        }
    }
    return true;
};

void make_new_block(void) { //���ο� ��� ����
    bx = (SCREEN_X / 2) - 1;
    by = 0;
    b_type = b_type_next;
    b_type_next = rand() % 7;
    b_rotation = 0;

    new_block_on = 0;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j] = ACTIVE_BLOCK; //�ʱ���ġ�� [bx,by]���� updata�� ����� ��� ���� ���
        }
    }
    for (i = 1; i < 3; i++) { //���ӻ���ǥ�ÿ� ������ ���ú��� �׸�
        for (j = 0; j < 4; j++) {
            if (blocks[b_type_next][0][i][j] == 1) {  //���� �׸��� ���´� b_type_next���� ù��° ȸ��������� ��.
                gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
                printf("��");
            }
            else {
                gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
                printf("  ");
            }
        }
    }
}

int move_block(int dir) {
    int tmp_rotation;
    switch (dir) {
    case LEFT:
        if(!check_crush(bx-1, by, b_rotation)) {Sleep(10); return 0;}
        block_clear();
        bx--;
        block_update();
        return 1;

    case RIGHT:
        if(!check_crush(bx+1, by, b_rotation)) {Sleep(10); return 0;}
        block_clear();
        bx++;
        block_update();
        return 1;

    case DOWN:
        if(!check_crush(bx, by+1, b_rotation)) {Sleep(10); return 0;}
        block_clear();
        by++;
        block_update();
        return 1;

    case UP: //Ű���� ���� �������� ȸ����Ŵ.
        tmp_rotation = (b_rotation + 1) % 4;
        if(!check_crush(bx, by, tmp_rotation)) {Sleep(10); return 0;}
        block_clear();
        b_rotation = tmp_rotation;
        block_update();
        return 1;

    case 100: //Lockdown Delay(���ٿ� ������)
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
        return 1;
    }
    return 0;
}

int GameIsValid() {
    for(i = 0; i < SCREEN_X; i++) {
        if(SCREEN_MEMORY[3][i] == INACTIVE_BLOCK) return false;
    }
    return true;
}

void user_cmd_scan(int cnt) {
    while(cnt--) { // �ϰ����� ����ϸ� Ű�Է¿� ���� �۵�
        int key = key_scanf();
        switch(key) {
            case IsArrowKey: //����Ű�Է� -> �����Է�
                move_block(key_scanf());
                break;
            case SPACE: //�� �̵�
                move_block(key);
                break;
            case ESC: //���α׷� ����
                exit(1);
                break;
            case PAUSE: //�Ͻ�����
                while(key_scanf() == PAUSE) {
                    Sleep(10);
                    //�ٽ� PAUSE���������� ���Ѵ��
                }
                break;
            default:
                break;
        }
        draw_game(SCREEN_UPDATE, SCREEN_MEMORY);
        Sleep(10);
    }
    return;
}

void block_clear() {
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if(SCREEN_UPDATE[by + i][bx + j] == ACTIVE_BLOCK)
                SCREEN_UPDATE[by + i][bx + j] = EMPTY;
        }
    }
}

void block_update() {
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j] = ACTIVE_BLOCK;
        }
    }
}

int count_score() {
    return 100 * clear_line(SCREEN_UPDATE, SCREEN_MEMORY, by);
}
