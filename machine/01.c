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

	//������ �о�, ����ü �迭 �ȿ� ���� ������.
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

	printf("=== ��带 �����Ͻʽÿ�. ===\n");
	printf("1. ����� ���\n");
	printf("2. ������ ���\n");
	printf("============================\n");
	printf("--> ");

	while (1) {
		scanf("%d", &mod);

		if (mod == 1) {
			printf("����� ��带 �����մϴ�. \n");
			break;
		}
		else if (mod == 2) {
			printf("������ ��带 �����մϴ�.. \n");
			break;
		}
		else {
			printf("�˸��� ���� �Է��Ͻʽÿ�. -->");
		}
	}

	// ����� ���
	if (mod == 1) {
		int money = 0;
	
		printf("�������� �Է��Ͻʽÿ�.(500�� �̻�) --> ");
		
		while (1) {
			scanf("%d", &money);

			if (money < 500) {
				printf("500�� �̻��� ���� �Է����ֽʽÿ�. \n");
			}
			else {
				break;
			}
		}

		int choice;

		printf("�޴��� �������ֽʽÿ�.\n");
		printf("============================\n");
		for (int i = 0; i < count; i++) {
			printf("%d) %s(%d��)(���: %d)\n", 
				product_arr[i].num, 
				product_arr[i].name, 
				product_arr[i].price, 
				product_arr[i].amount);
		}
		//printf("1. �ݶ�(500��)(���: ) ");
		//printf("2. ���̴�(600��)(���: ) ");
		//printf("3. ȯŸ(1000��)(���: ) ");
		printf("============================\n");
		printf("-->");

		scanf("%d", &choice);

		switch(choice) {
			case 1:
				if (money < 500) {
					printf("�ܾ��� �����մϴ�.");
					break;
				}
				money -= 500;
				printf("�ݶ� ���Խ��ϴ�. �ܾ�: %d��",money);
				break;
			case 2:
				if (money < 600) {
					printf("�ܾ��� �����մϴ�.");
					break;
				}

				money -= 600;
				printf("���̴ٰ� ���Խ��ϴ�. �ܾ�: %d��",money);
				break;
			case 3:
				if (money < 1000) {
					printf("�ܾ��� �����մϴ�.");
					break;
				}

				money -= 1000;
				printf("ȯŸ�� ���Խ��ϴ�. �ܾ�: %d��",money);
				break;
			default:
				printf("�� �� ���� �Է°��Դϴ�. \n");
				break;
		}

	}
	// ������ ���
	else if (mod == 2) {
		printf("������ ����Դϴ�.");
	}

	fclose(fp);

	return 0;
}