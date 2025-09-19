#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h> // malloc, free, exit �Լ� ����� ���� �߰�
#include <string.h> // strcmp �Լ� ����� ���� �߰�

#include <Windows.h>

HWND hwnd; // �ܼ� �ڵ�. ���� �ܼ� â�� ���� ���� �ܼ��� ���� ���� ���̶�� �� �� ����.
HDC hdc; // cmd ���� �׸��� �׸� ���� ��ɵ��� ������.

// �̹��� ����(����) ����
// enum�� 1, 2, 3, 4, 5, 6 �̷� ������ "������"���� �����͸� �����ϱ� ���� �����.
enum FORMAT { EMPTY, GREY, RGB, YCBCR, YCBCR420, BLOCK };

typedef struct imageType {
    unsigned int rows;      // �̹��� ���� ũ��(=����)
    unsigned int cols;      // �̹��� ���� ũ��(=����)
    char format;            // ������ ������ �̹��� ���� ����.
    unsigned long total;    // ��ü �ȼ� ��� ����
    unsigned int levels;    // �ִ� ��� ��.
    short* content;         // ���� �ȼ� �����Ͱ� ����Ǵ� ������
} ImageType;

typedef ImageType* Image;

// �̹����� ũ�� �����ϴ� ����.
Image imageAllocate(unsigned int rows, unsigned int cols, char format, unsigned int levels) {
    // �̹����� ũ�⸦ malloc �������� �ʿ��� ��ŭ�� ���� �Ҵ�. 
    Image im = malloc(sizeof(ImageType));
    im->rows = rows;
    im->cols = cols;
    im->format = format;
    im->levels = levels;
    im->content = NULL;
    if (im->format == EMPTY)
        return im;

    // �̹��� ���Ŀ� ���� total ��(=�̹����� ��ü �ȼ� ��)�� �ʿ��� ��ŭ �Ҵ�
    switch (im->format) {
    case GREY:
        im->total = im->cols * im->rows;
        break;
    case RGB:
    case YCBCR:
        im->total = 3 * im->cols * im->rows;
        break;
    case YCBCR420:
        im->total = 3 * im->cols * im->rows / 2;
        break;
    default:
        break;
    }

    // ���� ������ ���� ������ �̹��� ũ�⸸ŭ, content(=���� �ȼ� �����Ͱ� ����� ������)�� ũ�� �Ҵ�
    im->content = malloc(im->total * sizeof(short));

    return im;
}

// �޸𸮸� �����ϴ� �Լ�
void imageRelease(Image im) {
    im->format = EMPTY;
    if (im->content != NULL)
        free(im->content);
    im->content = NULL;
    free(im);
}

// PBM �̹����� �д� �ٽ� ����.
Image readPBMImage(const char* filename) {
    FILE* pgmFile;
    int k;

    char signature[3];
    unsigned cols, rows, levels;

    Image im = imageAllocate(0, 0, EMPTY, 0);

    pgmFile = fopen(filename, "rb");
    if (pgmFile == NULL) {
        perror("Cannot open file to read!");
        return im;
    }

    fgets(signature, sizeof(signature), pgmFile);
    if (strcmp(signature, "P5") != 0 && strcmp(signature, "P6") != 0 && strcmp(signature, "P4") != 0) {
        perror("Wrong file type!");
        fclose(pgmFile);
        return im;
    }

    fscanf(pgmFile, "%d %d %d", &cols, &rows, &levels);
    fgetc(pgmFile); // ����� ������ �ٹٲ� ���� ����

    // ������ �� �̹����� �����ϰ� ���� ũ�⿡ �°� �ٽ� �Ҵ�
    imageRelease(im);

    if (strcmp(signature, "P5") == 0 || strcmp(signature, "P4") == 0) {
        im = imageAllocate(rows, cols, GREY, levels);
        for (k = 0; k < im->total; ++k) {
            im->content[k] = (unsigned char)fgetc(pgmFile);
        }
    }
    else if (strcmp(signature, "P6") == 0) {
        im = imageAllocate(rows, cols, RGB, levels);
        unsigned long gOffset = im->cols * im->rows;
        unsigned long bOffset = 2 * im->cols * im->rows;
        for (k = 0; k < im->total / 3; ++k) {
            im->content[k] = (unsigned char)fgetc(pgmFile);
            im->content[k + gOffset] = (unsigned char)fgetc(pgmFile);
            im->content[k + bOffset] = (unsigned char)fgetc(pgmFile);
        }
    }

    fclose(pgmFile);
    return im;
}

void writePBMImage(const char* filename, const Image im) {
    FILE* pgmFile;
    int k;

    pgmFile = fopen(filename, "wb");
    if (pgmFile == NULL) {
        perror("Cannot open file to write");
        exit(-1);
    }

    if (im->format == GREY)
        fprintf(pgmFile, "P5\n");
    else if (im->format == RGB)
        fprintf(pgmFile, "P6\n");
    else {
        perror("Unknown file format\n");
        fclose(pgmFile);
        return;
    }

    fprintf(pgmFile, "%d %d\n", im->cols, im->rows);
    fprintf(pgmFile, "%d\n", im->levels);

    if (im->format == GREY) {
        for (k = 0; k < im->total; ++k)
            fputc((unsigned char)im->content[k], pgmFile);
    }
    else if (im->format == RGB) {
        unsigned long gOffset = im->cols * im->rows;
        unsigned long bOffset = 2 * im->cols * im->rows;
        for (k = 0; k < im->total / 3; ++k) {
            fputc((unsigned char)im->content[k], pgmFile);
            fputc((unsigned char)im->content[k + gOffset], pgmFile);
            fputc((unsigned char)im->content[k + bOffset], pgmFile);
        }
    }

    fclose(pgmFile);
}

void histogramEqualization(Image im) {
    // ��� �̹����� �ƴϰų�, �����Ͱ� ������ �Լ� ����
    if (im == NULL || im->format != GREY || im->content == NULL) {
        printf("Histogram equalization can only be applied to GREY images.\n");
        return;
    }


    // �̹����� �ּ�(minVal) / �ִ�(maxVal) �ȼ� �� ã��
    int minVal = im->levels; // �ִ� ��� ������ �ʱ�ȭ
    int maxVal = 0;          // 0���� �ʱ�ȭ
    unsigned long i;

    for (i = 0; i < im->total; ++i) {
        if (im->content[i] < minVal) {
            minVal = im->content[i];
        }
        if (im->content[i] > maxVal) {
            maxVal = im->content[i];
        }
    }

    // �̹����� ���� ������ ���, 0���� ������ ���� ����
    if (maxVal == minVal) {
        return;
    }

    // ����ȭ ���(normCoef) ���
    float normCoef = (float)im->levels / (maxVal - minVal);

    // ��Ȱȭ ������ ��� �ȼ��� ����
    for (i = 0; i < im->total; ++i) {
        // ����: (���� �ȼ��� - minVal) x normCoef
        // �ݿø��� ���� 0.5f�� ���� �� ���������� ��ȯ
        short newValue = (short)(((im->content[i] - minVal) * normCoef) + 0.5f);
        im->content[i] = newValue;
    }

}

//SetPixel -> Ư�� ��ġ�� ������ �׸��� �Լ�.
//cmd â�� �̹��� �׸��� �Լ�.
void DrawImage(Image im) {
    hwnd = GetForegroundWindow();
    hdc = GetWindowDC(hwnd);
    //Y�� -> X�� ������ ���� FOR���� ���� �׷���.
    //���ٺ��� �Ʒ��� ������ �������鼭(y ����), �� ���� ���ʿ��� ���������� �ȼ��� �׸��� ��(x����).
    for (int y = 0; y < im->rows; ++y) {
        for (int x = 0; x < im->cols; x++) {
            short pixelValue = im->content[y * im->cols + x];

            // ���� ����.
            // 250+x �̰��� ���� +250 ��ġ���� x, y�� �����ض� ��� �ǹ̶�� ���� ��.
            SetPixel(hdc, 250 + x, 250 + y, RGB(
                im->content[y * im->cols + x],
                im->content[y * im->cols + x],
                im->content[y * im->cols + x]));
        }
    }
}

// �̸� �״�� ������׷� �׸��� ���� �Լ�.
void DrawHistogram(Image im) {
    short* horizontal = malloc(sizeof(short) * im->cols);
    short* vertical = malloc(sizeof(short) * im->rows);

    for (int i = 0; i < im->cols; i++)
    {
        horizontal[i] = 0;
    }
    for (int i = 0; i < im->rows; i++)
    {
        vertical[i] = 0;
    }

    for (int y = 0; y < im->rows; ++y)
    {
        for (int x = 0; x < im->cols; x++)
        {
            if (im->content[y * im->cols + x] < 10)
            {
                horizontal[x]++;
            }
        }
    }
    for (int x = 0; x < im->cols; ++x)
    {
        for (int y = 0; y < im->rows; y++)
        {
            if (im->content[y * im->cols + x] < 10)
            {
                vertical[y]++;
            }
        }
    }

    hwnd = GetForegroundWindow();
    hdc = GetWindowDC(hwnd);
    for (int i = 0; i < im->cols; i++)
    {
        for (int j = 0; j < horizontal[i] * 20; j++)
        {
            SetPixel(hdc, 250 + i, 250 - j, RGB(0, 255, 0));
        }
    }
    for (int i = 0; i < im->rows; i++)
    {
        for (int j = 0; j < vertical[i] * 20; j++)
        {
            SetPixel(hdc, 250 - j, 250 + i, RGB(0, 255, 0));
        }
    }
    free(horizontal);
    free(vertical);
}

int main(void)
{
    // �� �ҽ��ڵ�� ���� �� Debug ���� x64�� x86���� �ٲ�� ��.
    // �̹����� null �����͸� �� ��. �̹����� ������ �����̴ϱ�, ���� 0���� �༭ �ƹ��͵� ����Ű�� ���� �ʴ�. ��� �ǹ̷� �� ��
    Image img = 0;

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            system("cls"); // ���� �ܼ� ���� ����� ����
            char imgPath[] = "./pgm/no0-1.pgm";
            imgPath[8] = '0' + i;
            imgPath[10] = '1' + j;
            img = readPBMImage(imgPath);
            DrawImage(img);
            DrawHistogram(img);
            imageRelease(img); // ���� ���� ������ �������༭, �޸� ������ ���´ٰ� ��.
            Sleep(10000);
        }
    }
    
    return 0;
}