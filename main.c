#include <stdio.h>
#include "RankingManage.h"

int main()
{
    init_DB();
    char id[ID_SIZE] = "ABCDDD";
    recording_and_print_ranking(id, 110); //new score랑 id 입력

    return 0;
}