#include <stdio.h>

int main(void) {
	// fopen��, �ش��ϴ� ������ ������ �����.
	FILE * fp = fopen("data2.txt", "wt");
	
	if (fp == NULL) {
		puts("���Ͽ��� ����!");
		return -1;
	}

	fputc('A', fp);
	fputc('B', fp);
	fputc('C', fp);

	fclose(fp);

	return 0;
}