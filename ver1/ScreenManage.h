#include <stdio.h>
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