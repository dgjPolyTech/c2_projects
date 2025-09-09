#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

// 음료 상품 정보를 저장할 구조체 선언
struct product {
	int num;
	char name[20];
	int price;
	int amount;
};

// 그 상품들의 목록을 저장할 구조체 배열 선언.(기본적으로 모두 0으로)
struct product p_arr[100] = { 0 };

void buying() {

}

int main() {
	int mod;

	FILE* fM = fopen("menus.txt", "rt");
	
	if (fM == NULL) {
		printf("파일을 찾을 수 없습니다. \n");
		return 1;
	}

	int ret;
	int p_size = 0; // 파일 길이 측정용

	while (1) {
		int readNum;
		char readName[20];
		int readPrice;
		int readAmount;

		ret = fscanf(fM, "%d %19s %d %d", &readNum, readName, &readPrice, &readAmount);
		
		if (ret == EOF) {
			break;
		}

		p_arr[p_size].num = readNum;
		strcpy(p_arr[p_size].name, readName);
		p_arr[p_size].price = readPrice;
		p_arr[p_size].amount = readAmount;

		p_size++;
		fclose(fM); // 올바른 곳에서 fclose 써주니까 코드 닫혔음. 내 원래 코드도 이런 문제 아닌지 확인해볼 것.
	}

	// 
	int count = 0;

	int choice;

	printf("===== 모드를 선택하십시오. =====\n");
	printf("1. 사용자 모드\n");
	printf("2. 관리자 모드\n");
	printf("================================\n");
	printf("--> ");

	while (1) {
		scanf("%d", &choice);
		
		if(choice == 1){
			mod = 1;
			break;
		} else if(choice == 2) {
			mod = 2;
			break;
		}
		else {
			printf("알맞은 값을 입력하세요.\n");
		}
	}

	// 사용자 모드
	if (mod == 1) {
		printf("사용자 모드 시작\n\n");
		while (1) {
			printf("=== 메뉴를 선택해주십시오. ===\n");
			for (int i = 0; i < p_size; i++) {
				printf("%d) %s(%d원)(재고량: %d)", p_arr[i].num, p_arr[i].name);
			}

			printf("==============================\n");
			break;
		}
	}

	//관리자 모드
	else if (mod == 2) {
		printf("관리자 모드 시작\n\n");
	}

	return 0;
}