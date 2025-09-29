#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

// �л��� ����� �迭, ��, ��, ��հ��� �޾� �л갪�� ����ϴ� �Լ�.
//int get_varience(int arr[][], int col, int row, int avg) {
//	int result = 0;
//
//	int curr_num = 0;
//
//	// ���� ���ϱ� = (�迭�� �� ��) - (��� ��)
//	for (int i = 0; i < col; i++) {
//		for (int j = 0; j < row; j++) {
//			curr_num = arr[i][j];
//			curr_num -= avg;
//			curr_num = curr_num * curr_num;
//		}
//	}
//
//
//
//	return result;
//
//}

int main(int argc, char* argv[]) {
	char str_tmp[1024];
	FILE* pFile = NULL;

	pFile = fopen("simple.csv", "r");

	int score_arr[10][4]; // �л� 10����� 
	int total_score = 0; // ��ü ����

	if (pFile != NULL)
	{

		// ������ �д� �ݺ���
		// ������ �� �� �̹Ƿ� �� 2�� �ݺ��ϰ� �ȴ�.
		int s_col = 0; // 2���� �迭�� ���� �� ���� �о�� �ϴ� �� �����ִ� ������ �Ѵ�.
		while ((fgets(str_tmp, 1024, pFile)) != NULL) {
			int s_row = 0;
			printf("%s\n", str_tmp); // �� ������ ���ڸ� �о�´�.
			char* p = strtok(str_tmp, ","); // ��ǥ(,)�� �������� ���ڿ��� �ڸ���.

			while (p != NULL) { // �� ���� null�� �ƴϸ�, 
				int number = atoi(p); // atoi = ���ĺ� �� ��Ʈ. ���ڸ� ���ڰ����� ��ȯ.
				score_arr[s_col][s_row] = number;
				printf("%d���� %d��° ��: %d\n", s_col, s_row, score_arr[s_col][s_row]);
				s_row++;
				p = strtok(NULL, ",");// strtok�� NULL���� ������, ������ ���ڿ��� �߶� �� ��ġ���� �ٽ� �߶󳻶�� �ǹ���. �̰� ������ �߶�´� ��ġ���� ��� �ٽ� �߶󳻴��� ������ �߻���.
			}
			s_col++;
		}

		// ������ ������ �迭�� �д� �ݺ���
		int arr_col = 0;
		int arr_row = 0;

		int score_total = 0; // ��ü ���� ����
		float score_avg = 0; // ���� ���
		float score_vari = 0; // ���� �л�(variance)

		// ���� ���� �� ��(=s_col)��ŭ�� �ݺ��Ѵ�.
		for (int i = 0; i < s_col; i++) {
			for (int j = 0; j < 4; j++) {
				score_total += score_arr[i][j];
			}
		}

		// ��ü ���� ����� ���Ѵ�.
		score_avg = (float)score_total / 8;

		float vari_total = 0;// �л� ��ü �� �޴� ����

		printf("��ü ���� ��� : %.2f\n", score_avg);
		// ��հ��� ���� ��ü ���� �л��� ���Ѵ�.
		for (int i = 0; i < s_col; i++) {
			for (int j = 0; j < 4; j++) {
				float curr_num = score_arr[i][j];
				curr_num -= score_avg;
				vari_total += curr_num * curr_num;
			}
		}

		printf("��ü ���� �л�: %.2f\n", vari_total / 8);

	}

	fclose(pFile);

	return 0;
}