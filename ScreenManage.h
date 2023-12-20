#include <stdio.h>
#include <string.h>
#include <windows.h>

//������ x,y��ǥũ�� ����

#define SCREEN_X 11 //����ȭ�� ����ũ��
#define SCREEN_Y 23 //����ȭ�� ����ũ��
#define SCREEN_X_ADJ 3 //����ȭ�� ��ġ����
#define SCREEN_Y_ADJ 1 //����ȭ�� ��ġ����

#define STATUS_X_ADJ SCREEN_X_ADJ+SCREEN_X+1 //��������ǥ�� ��ġ����

void test_map(char map[][SCREEN_X]);
int sum_line(char map[][SCREEN_X], int y);

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

void title(void) {
    int x=0, y=0;

    system("cls");
    gotoxy(x + 20, y + 4); printf("   2023 �ڷᱸ�� �⸻ ����   ");
    gotoxy(x + 20, y + 6); printf("   \"��Ʈ���� ����\"        ");

    gotoxy(x + 20, y + 9); printf("   <Menu>   ");
    gotoxy(x + 20, y + 10); printf("   1. ���� ����  ");
    gotoxy(x + 20, y + 11); printf("   2. ��ŷ        ");
    gotoxy(x + 20, y + 12); printf("   3. ���α׷� ����   ");
    gotoxy(x + 25, y + 15); printf("   ���μ�(1872301)       ");
    gotoxy(x + 25, y + 16); printf("   ������(2071333)   ");

    while (1) {
        if (kbhit()) {
            int key = key_scanf();

            if (key == '1') {
                break;
            } else if (key == '2') {
                system("cls");
                only_ranking_list();
                printf("�������� ���ư���(y or Y)");
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
                printf("���α׷� ����");
                exit(1);
            }
        }
    }

    while (kbhit()) getch(); // Clear input buffer
}

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
