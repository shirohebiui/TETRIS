#include <stdio.h>
#include <string.h>
#include <windows.h>

//������ x,y��ǥũ�� ����

#define SCREEN_X 11 //����ȭ�� ����ũ��
#define SCREEN_Y 23 //����ȭ�� ����ũ��
#define SCREEN_X_ADJ 3 //����ȭ�� ��ġ����
#define SCREEN_Y_ADJ 1 //����ȭ�� ��ġ����

#define STATUS_X_ADJ SCREEN_X_ADJ+SCREEN_X+1 //��������ǥ�� ��ġ����

void gotoxy(int x, int y) { //gotoxy�Լ�
    COORD pos = { 2 * x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void setcursortype() { //Ŀ������� �Լ�
    CONSOLE_CURSOR_INFO CurInfo;
    CurInfo.dwSize = 1;
    CurInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

//���� ��ü �������̽��� ǥ��
void draw_map() {          //���� ���� ǥ�ø� ��Ÿ���� �Լ�
    int y = 3;             // level, goal, score�� �����߿� ���� �ٲ�� �� ���� �� y���� ���� �����ص�
    // �׷��� Ȥ�� ���� ���� ǥ�� ��ġ�� �ٲ� �� �Լ����� �ȹٲ㵵 �ǰ�..
    gotoxy(STATUS_X_ADJ, y + 2); printf("* * N E X T * * ");
    gotoxy(STATUS_X_ADJ, y + 3); printf("*             * ");
    gotoxy(STATUS_X_ADJ, y + 4); printf("*             * ");
    gotoxy(STATUS_X_ADJ, y + 5); printf("*             * ");
    gotoxy(STATUS_X_ADJ, y + 6); printf("*             * ");
    gotoxy(STATUS_X_ADJ, y + 7); printf("* * * * * * * * ");
    gotoxy(STATUS_X_ADJ, y + 9); printf("  ��   : Shift        SPACE : Hard Drop");
    gotoxy(STATUS_X_ADJ, y + 10); printf("��  �� : Left / Right   P   : Pause");
    gotoxy(STATUS_X_ADJ, y + 11); printf("  ��   : Soft Drop     ESC  : Quit");
}



//����â ��ϵ��� ����

#define ACTIVE_BLOCK -2     // �����ǹ迭�� ����� ����� ���µ�
#define CEILLING -1         // ����� �̵��� �� �ִ� ������ 0 �Ǵ� ���� ������ ǥ��
#define EMPTY 0             // ����� �̵��� �� ���� ������ ����� ǥ��
#define WALL 1              // �� (�̵��� �� ���� ���)
#define INACTIVE_BLOCK 2    // �̵��� �Ϸ�� ��ϰ� (������ ���)

void draw_game(char SCREEN_UPDATE[][SCREEN_X], char SCREEN_MEMORY[][SCREEN_X]) { //������ �׸��� �Լ�
    int i, j;
    for (j = 1; j < SCREEN_X - 1; j++) { //õ���� ��� ���ο���� �������� �������� ���� �׷���
        if (SCREEN_UPDATE[3][j] == EMPTY) SCREEN_UPDATE[3][j] = CEILLING;
    }
    for (i = 0; i < SCREEN_Y; i++) {
        for (j = 0; j < SCREEN_X; j++) {
            if (SCREEN_MEMORY[i][j] != SCREEN_UPDATE[i][j]) { //UPDATE ����� ���ؼ� ���̰� �ִºκи� ȭ�鰻��
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
                case INACTIVE_BLOCK: //������ �� ���
                    printf("��");
                    break;
                case ACTIVE_BLOCK: //�����̰��ִ� �� ���
                    printf("��");
                    break;
                }
            }
        }
    }
    memmove(SCREEN_MEMORY, SCREEN_UPDATE, SCREEN_Y*SCREEN_X); //����ȭ���� ���� (mem <- update)
}