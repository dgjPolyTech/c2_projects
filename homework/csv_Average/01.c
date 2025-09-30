#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

//1���� �迭 �޾� ��հ��� ���ϴ� �Լ�. �迭�� �迭 �� ����� ����(��ǻ� ����)�� �޾� ����Ѵ�.
float get_average(int arr[], int count) {
	float result = 0;

	for (int i = 0; i < count; i++) {
		result += arr[i];
	}

	return result / count;
}

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
			printf("%s", str_tmp); // �� ������ ���ڸ� �о�´�.
			char* p = strtok(str_tmp, ","); // ��ǥ(,)�� �������� ���ڿ��� �ڸ���.

			while (p != NULL) { // �� ���� null�� �ƴϸ�, 
				int number = atoi(p); // atoi = ���ĺ� �� ��Ʈ. ���ڸ� ���ڰ����� ��ȯ.
				score_arr[s_col][s_row] = number;
				//printf("%d���� %d��° ��: %d\n", s_col, s_row, score_arr[s_col][s_row]);
				s_row++;
				p = strtok(NULL, ",");// strtok�� NULL���� ������, ������ ���ڿ��� �߶� �� ��ġ���� �ٽ� �߶󳻶�� �ǹ���. �̰� ������ �߶�´� ��ġ���� ��� �ٽ� �߶󳻴��� ������ �߻���.
			}
			s_col++;
		}
		printf("\n");

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
				curr_num -= score_avg; //����: ���� �迭�� ������ ��ո�ŭ ��
				vari_total += curr_num * curr_num; // �������� �����ؼ� �л� ��ü �� ������ �������.
			}
		}

		printf("��ü ���� �л�: %.2f\n\n", vari_total / 8);
		
		// �浿, ������ ���� ����� ���Ѵ�.(�浿�� ����, ������ �Ʒ���)
		float gil_avg = get_average(score_arr[0], 4);
		float chun_avg = get_average(score_arr[1], 4);

		printf("�浿�� ���� ���: %.2f\n", gil_avg);
		printf("������ ���� ���: %.2f\n\n", chun_avg);

		//���� ���� ����� ���Ѵ�.(����/����/����/���� ��)
		float kor_avg = (float)((score_arr[0][0] + score_arr[1][0]) / 2);
		float eng_avg = (float)((score_arr[0][1] + score_arr[1][1]) / 2);
		float math_avg = (float)((score_arr[0][2] + score_arr[1][2]) / 2);
		float science_avg = (float)((score_arr[0][3] + score_arr[1][3]) / 2);

		printf("���� ���� ���: %.2f\n", kor_avg);
		printf("���� ���� ���: %.2f\n", eng_avg);
		printf("���� ���� ���: %.2f\n", math_avg);
		printf("���� ���� ���: %.2f\n", science_avg);
	}

	fclose(pFile);

	return 0;
}