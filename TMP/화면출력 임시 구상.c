#include<stdio.h>
#include<windows.h>
#include<conio.h>
#include<time.h>
#include<stdlib.h>


#define LEFT 75 //�·� �̵�    //Ű���尪��
#define RIGHT 77 //��� �̵�
#define UP 72 //ȸ��
#define DOWN 80 //soft drop
#define SPACE 32 //hard drop
#define p 112 //�Ͻ�����
#define P 80 //�Ͻ�����
#define ESC 27 //��������

#define false 0
#define true 1

#define ACTIVE_BLOCK -2 // �����ǹ迭�� ����� ����� ���µ�
#define CEILLING -1     // ����� �̵��� �� �ִ� ������ 0 �Ǵ� ���� ������ ǥ��
#define EMPTY 0         // ����� �̵��� �� ���� ������ ����� ǥ��
#define WALL 1
#define INACTIVE_BLOCK 2 // �̵��� �Ϸ�� ��ϰ�

#define MAIN_X 11 //������ ����ũ��
#define MAIN_Y 23 //������ ����ũ��
#define MAIN_X_ADJ 3 //������ ��ġ����
#define MAIN_Y_ADJ 1 //������ ��ġ����

#define STATUS_X_ADJ MAIN_X_ADJ+MAIN_X+1 //��������ǥ�� ��ġ����

void gotoxy(int x, int y) { //gotoxy�Լ�
    COORD pos = { 2 * x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// �⺻���� : ����20 ����10
// + ���� ���δ� �ܺκ� 1ĭ ��Ŀ��
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


void draw_main(void) { //������ �׸��� �Լ�
    int i, j;
    // for (j = 1; j < MAIN_X - 1; j++) { //õ���� ��� ���ο���� �������� �������� ���� �׷���
    //     if (SCREEN_MEMORY[3][j] == EMPTY) SCREEN_MEMORY[3][j] = CEILLING;
    // }
    for (i = 0; i < SCREEN_H; i++) {
        for (j = 0; j < SCREEN_W; j++) {
            //if (SCREEN_MEMORY[i][j] != SCREEN_UPDATE[i][j]) { //cpy�� ���ؼ� ���� �޶��� �κи� ���� �׷���.
                //�̰� ������ ��������ü�� ��� �׷��� �������� ��¦�Ÿ�
                gotoxy(j,i);
                switch (SCREEN_UPDATE[i][j]) {
                case EMPTY: //��ĭ���
                    printf("  ");
                    break;
                case CEILLING: //õ����
                    printf(". ");
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