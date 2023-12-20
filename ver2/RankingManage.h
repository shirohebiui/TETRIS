//RANKING Manage 방법론
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define ID_SIZE 24 				//ID의 크기 임의 정의
#define DB_FILE "RANK_DB.txt" 	//FILE NAME 정의

FILE *read_fp = NULL;

//FILE에 저장할 규격
typedef struct DataBase {
    char id[ID_SIZE];
    int score;
} DataBase;

//DLL로 DATA관리할 생각
typedef struct double_node {
	struct DataBase DB;
	struct double_node *prev;
	struct double_node *next;
} node;

//기준이 되는 head, tail
node *head, *tail;

//node 초기화
void init_node() {
	head = (node*)malloc(sizeof(node));
	tail = (node*)malloc(sizeof(node));
	head->next = tail;
	head->prev = head;
	tail->next = tail;
	tail->prev = head;

}

//정렬된 위치에 삽입
void insert_node_into_sorted_idx(char *id, int score) {
	node *s = (node*)malloc(sizeof(node));
	node *i = (node*)malloc(sizeof(node));

	if(head->next == tail) {
		i->DB.score = score;
		strcpy(i->DB.id, id);
		i->prev = head->prev;
		i->next = tail->next;
		head->next = i;
		tail->prev = i;
	} else {
		s = head->next;
		while(s != tail) {
			if(s->DB.score < score) break;
			s = s->next;
		}
		i->DB.score = score;
		strcpy(i->DB.id, id);
		i->prev = s->prev;
		i->next = s;
		s->prev->next = i;
		s->prev = i;
	}
}

//DB 초기화 file에서 ranking을 읽어와 배열에 이중연결리스트에 저장
void init_DB() {
	init_node();
	struct DataBase rec;
	read_fp=fopen(DB_FILE, "r");
	while(fscanf(read_fp, "%s %d", rec.id, &rec.score) == 2) //2개의 인자를 읽어올 수 있냐로 유효성 판단해서 파일에서 읽어옴
	{
		//정렬된 데이터를 가져와 삽입함으로 정렬에 소요되는 오버헤드가없음
		//오직 메모리할당을 위한 오버헤드만 있을듯함
		insert_node_into_sorted_idx(rec.id, rec.score);
	}
	fclose(read_fp);
}

//전체 출력
void print_and_rec() {
    node *p = head->next;
	FILE *write_fp = fopen(DB_FILE, "wb"); //순위변동을 고려해 FILE을 처음부터 다시 쓰기위해서 wb모드로 받음
	int rank = 1;

	printf("\n-----RANKING-----\n");
	while(p != tail) {
		printf("[%d] \"%s\" %d\n", rank++, p->DB.id, p->DB.score);
		fprintf(write_fp, "%s %d\n", p->DB.id, p->DB.score);
		if(rank == 11) break; //11위부터는 필요 없음, 따라서 루프탈출
		p = p->next;
	}
	printf("\n");
	fclose(write_fp);
}

//mem free
void delete_nodes() {
	node *p = head;
	while(p != tail) { 	//head ~ tail->prev : FREE
		p = p->next;
		free(p->prev);
	}
	free(tail); 		//tail : FREE
}

//기록을 저장하고 동시에 최근 랭킹을 불러와서 출력함
void recording_and_print_ranking(char *id, int score) {
	insert_node_into_sorted_idx(id, score); //입력받은 점수를 정렬된 위치에 삽입
	print_and_rec(); 						//출력 & 파일저장
	delete_nodes(); 						//메모리정리
}
