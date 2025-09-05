#include <stdio.h>

int main(void) {
	// fopen시, 해당하는 파일이 없으면 만든다.
	FILE * fp = fopen("data2.txt", "wt");
	
	if (fp == NULL) {
		puts("파일오픈 실패!");
		return -1;
	}

	fputc('A', fp);
	fputc('B', fp);
	fputc('C', fp);

	fclose(fp);

	return 0;
}