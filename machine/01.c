#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

int main(void) {
	int mod;

	struct product {
		int num;
		char name[20];
		int price;
		int amount;
	};

	//파일을 읽어, 구조체 배열 안에 값을 저장함.
	FILE* fp = fopen("menus.txt", "rt");
	int ret;

	struct product product_arr[10] = { 0 };

	int count = 0;

	while (1) {
		int num;
		char name[20];
		int price;
		int amount;

		ret = fscanf(fp, "%d %s %d %d\n", &num, &name, &price, &amount);

		if (ret == EOF) {
			break;
		}

		product_arr[count].num = num;
		strcpy(product_arr[count].name, name);
		product_arr[count].price = price;
		product_arr[count].amount = amount;

		count++;
	}

	printf("=== 모드를 선택하십시오. ===\n");
	printf("1. 사용자 모드\n");
	printf("2. 관리자 모드\n");
	printf("============================\n");
	printf("--> ");

	while (1) {
		scanf("%d", &mod);

		if (mod == 1) {
			printf("사용자 모드를 시작합니다. \n");
			break;
		}
		else if (mod == 2) {
			printf("관리자 모드를 시작합니다.. \n");
			break;
		}
		else {
			printf("알맞은 값을 입력하십시오. -->");
		}
	}

	// 사용자 모드
	if (mod == 1) {
		int money = 0;
	
		printf("소지금을 입력하십시오.(500원 이상) --> ");
		
		while (1) {
			scanf("%d", &money);

			if (money < 500) {
				printf("500원 이상의 값을 입력해주십시오. \n");
			}
			else {
				break;
			}
		}

		int choice;

		printf("메뉴를 선택해주십시오.\n");
		printf("============================\n");
		for (int i = 0; i < count; i++) {
			printf("%d) %s(%d원)(재고량: %d)\n", 
				product_arr[i].num, 
				product_arr[i].name, 
				product_arr[i].price, 
				product_arr[i].amount);
		}
		//printf("1. 콜라(500원)(재고량: ) ");
		//printf("2. 사이다(600원)(재고량: ) ");
		//printf("3. 환타(1000원)(재고량: ) ");
		printf("============================\n");
		printf("-->");

		scanf("%d", &choice);

		switch(choice) {
			case 1:
				if (money < 500) {
					printf("잔액이 부족합니다.");
					break;
				}
				money -= 500;
				printf("콜라가 나왔습니다. 잔액: %d원",money);
				break;
			case 2:
				if (money < 600) {
					printf("잔액이 부족합니다.");
					break;
				}

				money -= 600;
				printf("사이다가 나왔습니다. 잔액: %d원",money);
				break;
			case 3:
				if (money < 1000) {
					printf("잔액이 부족합니다.");
					break;
				}

				money -= 1000;
				printf("환타가 나왔습니다. 잔액: %d원",money);
				break;
			default:
				printf("알 수 없는 입력값입니다. \n");
				break;
		}

	}
	// 관리자 모드
	else if (mod == 2) {
		printf("관리자 모드입니다.");
	}

	fclose(fp);

	return 0;
}