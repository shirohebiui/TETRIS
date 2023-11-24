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

// Ű���� �Է°� ����

#define LEFT 75 //->��ư - �·��̵�
#define RIGHT 77 //<-��ư - ����̵�
#define DOWN 80 //����ư - �ϰ�
#define UP 72 //߾��ư - ȸ��
#define SPACE 32 //space bar - �ٴڱ��� �ϰ�
#define ESC 27 //esc - ���α׷� ����

//����â ��ϵ��� ���� ���� !!

#define ACTIVE_BLOCK -2     // �����ǹ迭�� ����� ����� ���µ�
#define CEILLING -1         // ����� �̵��� �� �ִ� ������ 0 �Ǵ� ���� ������ ǥ��
#define EMPTY 0             // ����� �̵��� �� ���� ������ ����� ǥ��
#define WALL 1              // ��
#define INACTIVE_BLOCK 2    // �̵��� �Ϸ�� ��ϰ�


//***************** ���� ������ **************************************
int i,j;            //�ӽú��� (���־��Ƿ� �̸� �Ҵ�)

int b_type;         //��� ������ ����
int b_rotation;     //��� ȸ���� ����
int b_type_next;    //���� ��ϰ� ����

// ���� ���� ��������, �κ�Ž���� ����Ǵ°��� ���, ������Ʈ�� ���� �����̴�.
// ���� ������ �����ϴ°��� ������ char�� �������̹Ƿ� char�� �����Ͽ� ������带 ���̰�����
char SCREEN_UPDATE[SCREEN_Y][SCREEN_X]; //������Ʈ ������ ���
char SCREEN_MEMORY[SCREEN_Y][SCREEN_X]; //��� ȭ��

int bx, by; //�̵����� ����� �����ǻ��� x,y��ǥ�� ����

int key; //Ű����� �Է¹��� Ű���� ����

int speed; //��������ӵ�
int level; //���� level
int level_goal; //���������� �Ѿ�� ���� ��ǥ����
int cnt; //���� �������� ������ �� ���� ����
int score; //���� ����
int last_score = 0; //��������������
int best_score = 0; //�ְ��������

int new_block_on = 0; //���ο� ���� �ʿ����� �˸��� flag
int crush_on = 0; //���� �̵����� ����� �浹�������� �˷��ִ� flag
int level_up_on = 0; //���������� ����(���� ������ǥ�� �Ϸ�Ǿ�����) �˸��� flag
int space_key_on = 0; //hard drop�������� �˷��ִ� flag

// ************************ function *******************************

void reset(void); //������ �ʱ�ȭ
void reset_main(void); //���� ������(SCREEN_UPDATE[][]�� �ʱ�ȭ)
void reset_main_cpy(void); //copy ������(SCREEN_MEMORY[][]�� �ʱ�ȭ)
void draw_main(void); //�������� �׸�
void new_block(void); //���ο� ����� �ϳ� ����
void drop_block(void); //����� �Ʒ��� ����Ʈ��
int check_crush(int bx, int by, int rotation); //bx, by��ġ�� rotationȸ������ ���� ��� �浹 �Ǵ�
void move_block(int dir); //dir�������� ����� ������

/***************************************************************************************
****************************************************************************************/
int main(void)
{
    
    int i = 0;
    speed = 100;
    //while(1) {
        srand(time(NULL));  //���ο� ����� �����Ҷ� �������� ����� ���ϱ� ���� rand�Լ� ����ϱ� ���� �ڵ�
        setcursortype();    //Ŀ���� ���ִ� �Լ�����
        reset_main();       //�ʱ� ���� õ���� �׷��ִ� �Լ� ����
        draw_map();         //������ �����ִ� ������ ���� �׷��ִ� �Լ�
        draw_main();        //��ϵ��� ���º��� �׷��ִ� �Լ�  (SCREEN_MEMORY[i][j] �� SCREEN_UPDATE[i][j]�� ���ؼ� ����)

        b_type_next = rand() % 7;
        new_block();        //���ο� ����� ���Ӹ� ��ܿ� ��ġ��Ű�� ���� ����� �����ؼ� ����â�� �׷��ִ� �Լ�

        while (crush_on == 0) { //�ٴڿ� ���������� �̵���Ŵ
            draw_main();   //���ο� ���� ��� �׷���
            drop_block();  //���ǵ��� �����鼭 main_org�� ���� �������ش�. (�浹���� ������ y��������� 1ĭ�� �̵�)
            Sleep(100);    //C��� �����Լ� windows.h�����ϸ� ��밡�� (100 -> 0.1��)
        }
    //}
    gotoxy(20, 20);
    //system("cls");
    
    char id[ID_SIZE] = "USER1";     // ID�� �����Է¿�����, SCORE�� ���Ӱ���� ���� ����
    int score = 10000;              // ���� ���������� ���� �̱����̱⿡ �ӽ÷� �����ǰ��� ������
    system("cls");
    init_DB();
    gotoxy(5, 5); recording_and_print_ranking(id, score);
    while(1) {
        //���� �����Լ� �̱���
        //���� ���α׷�����
        Sleep(500);
    }
    

    return 0;
}
/***************************************************************************************
****************************************************************************************/

void draw_main(void) { //������ �׸��� �Լ�
    for (j = 1; j < SCREEN_X - 1; j++) { //õ���� ��� ���ο���� �������� �������� ���� �׷���
        if (SCREEN_UPDATE[3][j] == EMPTY) SCREEN_UPDATE[3][j] = CEILLING;
    }
    for (i = 0; i < SCREEN_Y; i++) {
        for (j = 0; j < SCREEN_X; j++) {
            if (SCREEN_MEMORY[i][j] != SCREEN_UPDATE[i][j]) { //UPDATE ����� ���ؼ� ���̰� �ִºκи� ȭ�鰻��
                //�̰� ������ ��������ü�� ��� �׷��� �������� ��¦�Ÿ�
                gotoxy(SCREEN_X_ADJ + j, SCREEN_Y_ADJ + i);
                switch (SCREEN_UPDATE[i][j]) {
                case EMPTY: //��ĭ���
                    printf("  ");
                    break;
                case CEILLING: //õ����
                    printf("- ");
                    break;
                case WALL: //�����
                    printf("��");
                    break;
                case INACTIVE_BLOCK: //���� �� ���
                    printf("��");
                    break;
                case ACTIVE_BLOCK: //�����̰��ִ� �� ���
                    printf("��");
                    break;
                }
            }
        }
    }
    memmove(SCREEN_MEMORY, SCREEN_UPDATE, sizeof(SCREEN_UPDATE)); //����ȭ���� ���� (mem <- update)
}

void reset_main() { //�������� �ʱ�ȭ
    // �������� 0���� �ʱ�ȭ
    memset(SCREEN_UPDATE, 0, sizeof(SCREEN_UPDATE));
    memset(SCREEN_MEMORY, 100, sizeof(SCREEN_UPDATE));
    
    //y���� 3�� ��ġ�� õ���� ����
    memset(&SCREEN_UPDATE[3][0], CEILLING, SCREEN_X);

    //����
    for (i = 1; i < SCREEN_Y - 1; i++) {
        SCREEN_UPDATE[i][0] = WALL;
        SCREEN_UPDATE[i][SCREEN_X - 1] = WALL;
    }

    //�ٴ�
    memset(&SCREEN_UPDATE[SCREEN_Y - 1][0], WALL, SCREEN_X);
}

void drop_block() {
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

void move_block(int dir) { //����� �̵���Ŵ
    switch (dir) {
    case LEFT: //���ʹ���
        for (i = 0; i < 4; i++) { //������ǥ�� ���� ����
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j] = EMPTY;
            }
        }
        for (i = 0; i < 4; i++) { //�������� ��ĭ���� active block�� ����
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j - 1] = ACTIVE_BLOCK;
            }
        }
        bx--; //��ǥ�� �̵�
        break;

    case RIGHT:    //������ ����. ���ʹ����̶� ���� ������ ����
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

    case DOWN:    //�Ʒ��� ����. ���ʹ����̶� ���� ������ ����
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

    case UP: //Ű���� ���� �������� ȸ����Ŵ.
        for (i = 0; i < 4; i++) { //������ǥ�� ���� ����
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j] = EMPTY;
            }
        }
        b_rotation = (b_rotation + 1) % 4; //ȸ������ 1������Ŵ(3���� 4�� �Ǵ� ���� 0���� �ǵ���)
        for (i = 0; i < 4; i++) { //ȸ���� ����� ����
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j] = ACTIVE_BLOCK;
            }
        }
        break;

    case 100: //����� �ٴ�, Ȥ�� �ٸ� ��ϰ� ���� ���¿��� ��ĭ���� �÷� ȸ���� ������ ���
        //�̸� ���۽�Ű�� Ư������
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

int check_crush(int bx, int by, int b_rotation) { //������ ��ǥ�� ȸ�������� �浹�� �ִ��� �˻�
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) { //������ ��ġ�� �����ǰ� ������� ���ؼ� ��ġ�� false�� ����
            if (blocks[b_type][b_rotation][i][j] == 1 && SCREEN_UPDATE[by + i][bx + j] > 0) return false;
        }
    }
    return true; //�ϳ��� �Ȱ�ġ�� true����
};

void new_block(void) { //���ο� ��� ����
    bx = (SCREEN_X / 2) - 1; //��� ���� ��ġx��ǥ(�������� ���)
    by = 0;  //��� ������ġ y��ǥ(���� ��)
    b_type = b_type_next; //���������� ������
    b_type_next = rand() % 7; //���� ���� ����
    b_rotation = 0;  //ȸ���� 0������ ������

    new_block_on = 0; //new_block flag�� ��

    for (i = 0; i < 4; i++) { //������ bx, by��ġ�� ������
        for (j = 0; j < 4; j++) {
            if (blocks[b_type][b_rotation][i][j] == 1) SCREEN_UPDATE[by + i][bx + j] = ACTIVE_BLOCK;
        }
    }
    for (i = 1; i < 3; i++) { //���ӻ���ǥ�ÿ� ������ ���ú��� �׸�
        for (j = 0; j < 4; j++) {
            if (blocks[b_type_next][0][i][j] == 1) {
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