//RANKING Manage �����
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define ID_SIZE 24 				//ID�� ũ�� ���� ����
#define DB_FILE "RANK_DB.txt" 	//FILE NAME ����

FILE *read_fp = NULL;

//FILE�� ������ �԰�
typedef struct DataBase {
    char id[ID_SIZE];
    int score;
} DataBase;

//DLL�� DATA������ ����
typedef struct double_node {
	struct DataBase DB;
	struct double_node *prev;
	struct double_node *next;
} node;

//������ �Ǵ� head, tail
node *head, *tail;

//node �ʱ�ȭ
void init_node() {
	head = (node*)malloc(sizeof(node));
	tail = (node*)malloc(sizeof(node));
	head->next = tail;
	head->prev = head;
	tail->next = tail;
	tail->prev = head;

}

//���ĵ� ��ġ�� ����
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

//DB �ʱ�ȭ file���� ranking�� �о�� �迭�� ���߿��Ḯ��Ʈ�� ����
void init_DB() {
	init_node();
	struct DataBase rec;
	read_fp=fopen(DB_FILE, "r");
	while(fscanf(read_fp, "%s %d", rec.id, &rec.score) == 2) //2���� ���ڸ� �о�� �� �ֳķ� ��ȿ�� �Ǵ��ؼ� ���Ͽ��� �о��
	{
		//���ĵ� �����͸� ������ ���������� ���Ŀ� �ҿ�Ǵ� ������尡����
		//���� �޸��Ҵ��� ���� ������常 ��������
		insert_node_into_sorted_idx(rec.id, rec.score);
	}
	fclose(read_fp);
}

//��ü ���
void print_and_rec() {
    node *p = head->next;
	FILE *write_fp = fopen(DB_FILE, "wb"); //���������� ����� FILE�� ó������ �ٽ� �������ؼ� wb���� ����
	int rank = 1;

	printf("\n-----RANKING-----\n");
	while(p != tail) {
		printf("[%d] \"%s\" %d\n", rank++, p->DB.id, p->DB.score);
		fprintf(write_fp, "%s %d\n", p->DB.id, p->DB.score);
		if(rank == 11) break; //11�����ʹ� �ʿ� ����, ���� ����Ż��
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

//����� �����ϰ� ���ÿ� �ֱ� ��ŷ�� �ҷ��ͼ� �����
void recording_and_print_ranking(char *id, int score) {
	insert_node_into_sorted_idx(id, score); //�Է¹��� ������ ���ĵ� ��ġ�� ����
	print_and_rec(); 						//��� & ��������
	delete_nodes(); 						//�޸�����
}
