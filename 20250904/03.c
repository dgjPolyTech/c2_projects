#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>


int main(void) {
	char name[10];
	char sex;
	int age;

	// 파일에 내용 입력하기.
	//FILE* fp = fopen("friend.txt", "wt");
	//int i;

	//for (i = 0; i < 3; i++) {
	//	printf("이름 성별 나이 순 입력: ");
	//	scanf("%s %c %d", name, &sex, &age);
	//	getchar();
	//	fprintf(fp, "%s %c %d", name, sex, age);
	//}

	// 파일 읽어 내용 출력하기
	FILE* fp = fopen("friend.txt", "rt");
	int ret;

	while (1) {
		ret = fscanf(fp, "%s %c %d", name, &sex, &age);
		if (ret == EOF)
			break;
		printf("%s %c %d \n", name, sex, age);
	}

	fclose(fp);
	return 0;
}