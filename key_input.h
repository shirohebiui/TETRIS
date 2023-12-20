// 키보드 입력 방법
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

#define LEFT 75 //->버튼 - 좌로이동
#define RIGHT 77 //<-버튼 - 우로이동
#define DOWN 80 //下버튼 - 하강
#define UP 72 //上버튼 - 회전
#define SPACE 32 //space bar - 바닥까지 하강
#define ESC 27 //esc - 프로그램 종료
#define PAUSE 112 //P - 일시정지, 정지해제
#define PAUSE2 80 //p 80
#define IsArrowKey 224 //방향키

int key_scanf(void) { //키값을 읽어서 리턴
    int key = 0; //키값 초기화

    if (kbhit()) { //키 입력이 있으면1 없으면0 리턴 #conio.h lib func
        key = getch(); //입력버퍼 있으면 읽기
    fflush(stdin); //입력버퍼 비우기

    return key;
    }
}
