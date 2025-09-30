#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

//1차원 배열 받아 평균값을 구하는 함수. 배열과 배열 안 요소의 개수(사실상 길이)를 받아 계산한다.
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

	int score_arr[10][4]; // 학생 10명까지 
	int total_score = 0; // 전체 점수

	if (pFile != NULL)
	{

		// 파일을 읽는 반복문
		// 파일이 두 줄 이므로 총 2번 반복하게 된다.
		int s_col = 0; // 2차원 배열이 현재 몇 층을 읽어야 하는 지 정해주는 역할을 한다.
		while ((fgets(str_tmp, 1024, pFile)) != NULL) {
			int s_row = 0;
			printf("%s", str_tmp); // 한 라인의 문자를 읽어온다.
			char* p = strtok(str_tmp, ","); // 쉼표(,)를 기준으로 문자열을 자른다.

			while (p != NULL) { // 그 값이 null이 아니면, 
				int number = atoi(p); // atoi = 알파벳 투 인트. 문자를 숫자값으로 반환.
				score_arr[s_col][s_row] = number;
				//printf("%d행의 %d번째 값: %d\n", s_col, s_row, score_arr[s_col][s_row]);
				s_row++;
				p = strtok(NULL, ",");// strtok에 NULL값을 넣으면, 이전에 문자열을 잘라낸 그 위치에서 다시 잘라내라는 의미임. 이게 없으면 잘라냈던 위치에서 계속 다시 잘라내느라 오류가 발생함.
			}
			s_col++;
		}
		printf("\n");

		// 점수를 저장한 배열을 읽는 반복문
		int arr_col = 0;
		int arr_row = 0;

		int score_total = 0; // 전체 점수 총합
		float score_avg = 0; // 점수 평균
		float score_vari = 0; // 점수 분산(variance)

		// 점수 정보 줄 수(=s_col)만큼만 반복한다.
		for (int i = 0; i < s_col; i++) {
			for (int j = 0; j < 4; j++) {
				score_total += score_arr[i][j];
			}
		}

		// 전체 점수 평균을 구한다.
		score_avg = (float)score_total / 8;

		float vari_total = 0;// 분산 전체 값 받는 변수

		printf("전체 점수 평균 : %.2f\n", score_avg);

		// 평균값을 토대로 전체 점수 분산을 구한다.
		for (int i = 0; i < s_col; i++) {
			for (int j = 0; j < 4; j++) {
				float curr_num = score_arr[i][j];
				curr_num -= score_avg; //편차: 현재 배열의 수에서 평균만큼 뺌
				vari_total += curr_num * curr_num; // 편차값을 제곱해서 분산 전체 합 변수에 집어넣음.
			}
		}

		printf("전체 점수 분산: %.2f\n\n", vari_total / 8);
		
		// 길동, 춘향의 점수 평균을 구한다.(길동이 윗줄, 춘향이 아랫줄)
		float gil_avg = get_average(score_arr[0], 4);
		float chun_avg = get_average(score_arr[1], 4);

		printf("길동의 점수 평균: %.2f\n", gil_avg);
		printf("춘향의 점수 평균: %.2f\n\n", chun_avg);

		//과목별 점수 평균을 구한다.(국어/영어/수학/과학 순)
		float kor_avg = (float)((score_arr[0][0] + score_arr[1][0]) / 2);
		float eng_avg = (float)((score_arr[0][1] + score_arr[1][1]) / 2);
		float math_avg = (float)((score_arr[0][2] + score_arr[1][2]) / 2);
		float science_avg = (float)((score_arr[0][3] + score_arr[1][3]) / 2);

		printf("국어 점수 평균: %.2f\n", kor_avg);
		printf("영어 점수 평균: %.2f\n", eng_avg);
		printf("수학 점수 평균: %.2f\n", math_avg);
		printf("과학 점수 평균: %.2f\n", science_avg);
	}

	fclose(pFile);

	return 0;
}