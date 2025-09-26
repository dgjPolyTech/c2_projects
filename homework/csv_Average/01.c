#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
	char str_tmp[1024];
	FILE* pFile = NULL;

	pFile = fopen("simple.csv", "r");

	int score_gil[4];
	int score_chun[4];

	if (pFile != NULL)
	{
		/*while (1) {
			char* ret = fgets(str_tmp, 1024, pFile);
			if (ret == NULL) {
				break;
			}
			printf("%s", str_tmp);
		}*/

		/*while ((fgets(str_tmp, 1024, pFile)) != NULL) {
			printf("%s\n", str_tmp);
			char* p = strtok(str_tmp, ",");
			while (p != NULL) {
				printf("%s\n", p);
				p = strtok(NULL, ",");
			}
		}*/

		while ((fgets(str_tmp, 1024, pFile)) != NULL) {
			printf("%s\n", str_tmp);
			char* p = strtok(str_tmp, ",");
			while (p != NULL) {
				int number = atoi(p);
				printf("%d\n", number);
				p = strtok(NULL, ",");
			}
		}
	}

	

	fclose(pFile);
	
	return 0;
}