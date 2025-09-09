#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// 구조체 정의는 동일합니다.
struct product {
    int num;
    char name[20];
    int price;
    int amount;
};

int money = 0;

void saveData(struct product* parr, int size) {
    FILE* fp = fopen("menus.txt", "wt");
    if (fp == NULL) {
        printf("파일 저장에 실패했습니다.\n");
        return;
    }

    for (int i = 0; i < size; i++) {
        fprintf(fp, "%d %s %d %d\n", parr[i].num, parr[i].name, parr[i].price, parr[i].amount);
    }

    fclose(fp);
    printf("변경된 내용이 menus.txt 파일에 저장되었습니다.\n");
}


void addProduct() {
    printf("제품 추가 기능은 아직 구현되지 않았습니다.\n");
}

void buy(int u_num, struct product* parr, int size) {
    bool isIN = false; // 찾는 제품이 있는지 확인

    for (int i = 0; i < size; i++) {
        if (parr[i].num == u_num) {
            isIN = true;

            if (money < parr[i].price) {
                printf("잔액이 부족합니다.\n");
                return; // 함수 종료
            }
            if (parr[i].amount > 0) {
                parr[i].amount -= 1; // 메모리 상의 데이터만 변경
                money -= parr[i].price;
                printf("\n>> %s가 나왔습니다. 남은 금액: %d원 <<\n\n", parr[i].name, money);
            }
            else {
                printf("%s는 재고가 없습니다.\n", parr[i].name);
            }
            return; // 제품을 찾았으므로 더 이상 반복할 필요 없음
        }
    }

    if (!isIN) {
        printf("%d번에 해당하는 음료가 없습니다. \n", u_num);
    }
}

int main(void) {
    int mod;

    // 파일을 읽어 구조체 배열 안에 값을 저장함.
    FILE* fp = fopen("menus.txt", "rt");

    if (fp == NULL) {
        printf("menus.txt 파일을 열 수 없습니다. 프로그램을 종료합니다.\n");
        return 1;
    }

    struct product product_arr[10] = { 0 };
    int count = 0;

    // 파일에서 데이터 읽기 (버퍼 오버플로우 방지)
    while (count < 10 && fscanf(fp, "%d %19s %d %d", &product_arr[count].num, product_arr[count].name, &product_arr[count].price, &product_arr[count].amount) != EOF) {
        count++;
    }

    // 데이터를 다 읽었으면 파일을 바로 닫아줌
    fclose(fp);

    printf("=== 모드를 선택하십시오. ===\n");
    printf("1. 사용자 모드\n");
    printf("2. 관리자 모드\n");
    printf("============================\n");
    printf("--> ");

    while (1) {
        scanf("%d", &mod);

        if (mod == 1 || mod == 2) {
            break;
        }
        else {
            printf("1 또는 2를 입력하십시오. --> ");
        }
    }

    // 사용자 모드
    if (mod == 1) {
        printf("사용자 모드를 시작합니다. \n");
        printf("소지금을 입력하십시오.(500원 이상) --> ");

        while (1) {
            scanf("%d", &money);
            if (money < 500) {
                printf("500원 이상의 값을 입력해주십시오. --> ");
            }
            else {
                break;
            }
        }

        while (1) {
            int choice;

            printf("메뉴를 선택해주십시오.(현재 소지금: %d원)\n", money);
            printf("종료: -1 입력\n");
            printf("============================\n");
            for (int i = 0; i < count; i++) {
                printf("%d) %s (%d원) [재고: %d개]\n",
                    product_arr[i].num,
                    product_arr[i].name,
                    product_arr[i].price,
                    product_arr[i].amount);
            }
            printf("============================\n");
            printf("--> ");

            scanf("%d", &choice);

            if (choice == -1) {
                printf("프로그램을 종료합니다. 잔액: %d원\n", money);
                break;
            }

            buy(choice, product_arr, count);
        }
    }
    // 관리자 모드
    else if (mod == 2) {
        // 관리자 모드 로직 (현재는 미구현 상태)
        printf("관리자 모드는 아직 구현되지 않았습니다.\n");
    }

    // 프로그램이 정상적으로 종료되기 직전, 변경된 내용을 파일에 저장
    saveData(product_arr, count);

    return 0;
}