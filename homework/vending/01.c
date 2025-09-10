#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// 음료 상품 정보를 저장할 구조체 선언
struct product {
    int num;
    char name[20];
    int price;
    int amount;
};

struct time {
    char name[20];
    long date;
    int hour;
    int minute;
};

// 그 상품들의 목록을 저장할 구조체 배열 선언.(기본적으로 모두 0으로)
struct product p_arr[100] = { 0 };

// 기본 소지금(사용자 모드 시)
int money = 0;

// 파일 덮어쓰기 전담 함수
void editFile(int fMod, struct product* p_arr, int m_size) {

    // menus.txt 파일 덮어쓰기
    if (fMod = 1) {
        FILE* fp;

        fp = fopen("menus.txt", "wt");

        if (fp == NULL) {
            printf("파일을 열 수 없습니다. \n");
        }

        for (int i = 0; i < m_size; i++) {
            fprintf(fp, "%d %s %d %d\n", p_arr[i].num, p_arr[i].name, p_arr[i].price, p_arr[i].amount);
        }

        fclose(fp);
    }
}

// 사용자 음료 구매 함수
void buying(int choice, struct product* p_arr, int m_size) {
    // write 모드로 파일 열기
    bool isIN = false; // bool 함수 사용할때는 stdbool 라이브러리 include 필요함.

    // menus 배열 크기만큼 반복
    for (int i = 0; i < m_size; i++) {

        if (p_arr[i].num == choice) {
            isIN = true; // 메뉴를 찾았으니 true;

            // 소지금이 가격 이상이면,
            if (money >= p_arr[i].price) {
                if (p_arr[i].amount <= 0) {
                    // 재고가 0 이하라면, 재고가 없습니다. 출력하고 넘어감.
                    printf("%s 재고가 없습니다. \n", p_arr[i].name);
                }
                else {
                    p_arr[i].amount -= 1;
                    money = money - p_arr[i].price;
                    // at(AddText) 모드로 파일 오픈
                    FILE* buyInfo = fopen("buyInfo.txt", "at");

                    if (buyInfo != NULL) {
                        time_t t = time(NULL);
                        struct time* tm_info = localtime(&t);

                        char date_str[20];
                        char time_str[20];
                        strftime(date_str, sizeof(date_str), "%Y%m%d", tm_info);
                        strftime(time_str, sizeof(time_str), "%H:%M", tm_info);

                        fprintf(buyInfo, "%s %s %s\n", 
                                        p_arr[i].name,
                                        date_str,
                                        time_str);
                        fclose(buyInfo);
                    }
                }
            }
            else {
                printf("소지금이 부족합니다.\n");
            }
        }
    }

    if (!isIN) {
        printf("%d에 해당하는 제품이 없습니다. \n", choice);
    }

    editFile(1, p_arr, m_size);
}

// 메뉴 수정(재고추가/삭제 등)
void editMenu(int choice, struct product *p_arr, int m_size) {
    //1. 새 상품 추가
    if (choice == 1) {
        char newName[20];
        int newPrice;
        int newAmount;

        printf("신규 상품 추가\n");
        printf("새 상품명 입력: ");
        scanf("%19s", newName);
        printf("새 상품 가격 입력: ");
        scanf("%d", &newPrice);
        printf("새 상품 재고량 입력: ");
        scanf("%d", &newAmount);

        // 가장 마지막에 추가해야 하므로, m_size로 자릿수 지정
        p_arr[m_size].num = m_size + 1;
        strcpy(p_arr[m_size].name, newName);
        p_arr[m_size].price = newPrice;
        p_arr[m_size].amount = newAmount;

        // 원본 m_size +1
        m_size++;

        editFile(1, p_arr, m_size);
    }//2. 기존 상품 정보 수정
    else if (choice == 2) {
        int newNum;
        char newName[20];
        int newPrice;
        int newAmount;

        printf("상품 정보 수정\n");
        printf("수정할 상품 번호 입력(수정을 원치 않을 시, Enter): ");
        scanf("%d", &newNum);
        printf("상품명 입력: ");
        scanf("%19s", &newName);
        printf("상품가격 입력: ");
        scanf("%d", &newPrice);
        printf("상품 재고량 입력: ");
        scanf("%d", &newAmount);

        for (int i = 0; i < m_size; i++) {
            if (newNum == p_arr[i].num) {
                if (newName != "") { strcpy(p_arr[i].name, newName); };
                if (newPrice != "") { p_arr[i].price == newPrice; };
                if (newAmount != "") { p_arr[i].amount == newAmount; };
                break;
            }
        }

        editFile(1, p_arr, m_size);

    }//3. 기존 상품 삭제(삭제할 상품 뒷 번호의 정보로 덮어쓰는 방식으로 구현.
    else if (choice == 3) {
        int delNum;
        char newName[20];
        int newPrice;
        int newAmount;

        printf("상품 정보 삭제\n");
        printf("삭제할 상품 번호 입력: ");
        scanf("%d", &delNum);

        int delIdx;
        bool isIN = false;

        // 삭제할 상품의 인덱스 찾기
        for (int i = 0; i < m_size; i++) {
            if (p_arr[i].num == delNum) {
                delIdx = i;
                isIN = true;
            }
        }

        // 인덱스 찾았으면
        if (isIN) {
            for (int i = delIdx; i < m_size -1; i++) {
                p_arr[i] = p_arr[i + 1]; // 뒷번호 인덱스의 정보를 앞으로 끌어옴.
                p_arr[i].num = i + 1; // 그에 맞춰 번호 부여
            }
        }

        m_size--;

        editFile(1, p_arr, m_size);
        printf("삭제가 완료되었습니다.");
    }
}

// 판매내역 조회
void buyInfo() {
    FILE* fp = fopen("buyInfo.txt", "rt");
    int ret;

    struct time ts;

    while (1) {
        ret = fscanf(fp, "%19s %ld %d:%d", ts.name, &ts.date, &ts.hour, &ts.minute);
        if (ret == EOF) {
            break;
        }
        printf("%ld %d:%d)%s가 판매되었습니다.\n", ts.date, ts.hour, ts.minute, ts.name);
    }

    fclose(fp);
}

int main() {
    int mod;

    FILE* fM = fopen("menus.txt", "rt");

    if (fM == NULL) {
        printf("파일을 찾을 수 없습니다. \n");
        return 1;
    }

    int ret;
    int m_size = 0; // menus 파일 길이 측정용

    while (1) {
        int readNum;
        char readName[20];
        int readPrice;
        int readAmount;

        ret = fscanf(fM, "%d %s %d %d", &readNum, readName, &readPrice, &readAmount);

        if (ret == EOF) {
            break;
        }

        p_arr[m_size].num = readNum;
        strcpy(p_arr[m_size].name, readName);
        p_arr[m_size].price = readPrice;
        p_arr[m_size].amount = readAmount;

        m_size++;
    }

    fclose(fM); // 올바른 곳에서 fclose 써주니까 코드 닫혔음. 내 원래 코드도 이런 문제 아닌지 확인해볼 것.

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

        if (choice == 1) {
            mod = 1;
            break;
        }
        else if (choice == 2) {
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

        printf("소지금을 입력해주십시오. --> ");
        scanf("%d", &money);

        while (1) {
            int choice;
            printf("=== 메뉴를 선택해주십시오.(소지금: %d원) ===\n", money);
            for (int i = 0; i < m_size; i++) {
                printf("%d) %s(%d원)(재고량: %d)\n", p_arr[i].num, p_arr[i].name, p_arr[i].price, p_arr[i].amount);
            }
            printf("==============================\n");
            printf("(종료: -1 입력) --> ");
            scanf("%d", &choice);

            if (choice == -1) {
                printf("프로그램 종료");
                break;
            }

            buying(choice, p_arr, m_size);
        }
    }

    //관리자 모드
    else if (mod == 2) {
        printf("관리자 모드 시작\n\n");
        
        while (1) {
            int choice;

            printf("=== 원하시는 기능을 입력해주십시오. ===\n");
            printf("1) 새 상품 추가\n");//wt
            printf("2) 기존 상품 정보 수정\n");//wt
            printf("3) 기존 상품 삭제\n"); //wt
            printf("4) 상품 판매 내역 조회\n");
            printf("=======================================\n");
            printf("(종료: -1 입력) --> ");

            scanf("%d", &choice);

            if (choice == -1) {
                printf("프로그램 종료 \n");
                break;
            }

            if (choice == 1 || choice == 2 || choice == 3) {
                editMenu(choice, p_arr, m_size);
            } else if (choice == 4) {
                buyInfo();
            }
            else {
                printf("알맞은 옵션을 입력해주십시오. \n");
            }
        }
    }

    return 0;
}