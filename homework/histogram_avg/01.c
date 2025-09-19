#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h> // malloc, free, exit 함수 사용을 위해 추가
#include <string.h> // strcmp 함수 사용을 위해 추가

#include <Windows.h>

HWND hwnd; // 콘솔 핸들. 대충 콘솔 창에 띄우기 위한 콘솔의 값을 가진 것이라고 볼 수 있음.
HDC hdc; // cmd 위에 그림을 그릴 각종 기능들을 포함함.

// 이미지 유형(포맷) 모음
// enum은 1, 2, 3, 4, 5, 6 이런 식으로 "열거형"으로 데이터를 저장하기 위해 사용함.
enum FORMAT { EMPTY, GREY, RGB, YCBCR, YCBCR420, BLOCK };

typedef struct imageType {
    unsigned int rows;      // 이미지 세로 크기(=높이)
    unsigned int cols;      // 이미지 가로 크기(=넓이)
    char format;            // 위에서 정의한 이미지 포맷 정보.
    unsigned long total;    // 전체 픽셀 요소 개수
    unsigned int levels;    // 최대 밝기 값.
    short* content;         // 실제 픽셀 데이터가 저장되는 포인터
} ImageType;

typedef ImageType* Image;

// 이미지의 크기 정의하는 구간.
Image imageAllocate(unsigned int rows, unsigned int cols, char format, unsigned int levels) {
    // 이미지의 크기를 malloc 형식으로 필요한 만큼만 동적 할당. 
    Image im = malloc(sizeof(ImageType));
    im->rows = rows;
    im->cols = cols;
    im->format = format;
    im->levels = levels;
    im->content = NULL;
    if (im->format == EMPTY)
        return im;

    // 이미지 형식에 따라 total 값(=이미지의 전체 픽셀 값)을 필요한 만큼 할당
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

    // 위의 과정을 거쳐 구해진 이미지 크기만큼, content(=실제 픽셀 데이터가 저장될 포인터)의 크기 할당
    im->content = malloc(im->total * sizeof(short));

    return im;
}

// 메모리를 해제하는 함수
void imageRelease(Image im) {
    im->format = EMPTY;
    if (im->content != NULL)
        free(im->content);
    im->content = NULL;
    free(im);
}

// PBM 이미지를 읽는 핵심 구간.
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
    fgetc(pgmFile); // 헤더의 마지막 줄바꿈 문자 제거

    // 기존의 빈 이미지를 해제하고 실제 크기에 맞게 다시 할당
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
    // 흑백 이미지가 아니거나, 데이터가 없으면 함수 종료
    if (im == NULL || im->format != GREY || im->content == NULL) {
        printf("Histogram equalization can only be applied to GREY images.\n");
        return;
    }


    // 이미지의 최소(minVal) / 최대(maxVal) 픽셀 값 찾기
    int minVal = im->levels; // 최대 밝기 값으로 초기화
    int maxVal = 0;          // 0으로 초기화
    unsigned long i;

    for (i = 0; i < im->total; ++i) {
        if (im->content[i] < minVal) {
            minVal = im->content[i];
        }
        if (im->content[i] > maxVal) {
            maxVal = im->content[i];
        }
    }

    // 이미지가 단일 색상일 경우, 0으로 나누기 오류 방지
    if (maxVal == minVal) {
        return;
    }

    // 정규화 계수(normCoef) 계산
    float normCoef = (float)im->levels / (maxVal - minVal);

    // 평활화 공식을 모든 픽셀에 적용
    for (i = 0; i < im->total; ++i) {
        // 공식: (기존 픽셀값 - minVal) x normCoef
        // 반올림을 위해 0.5f를 더한 후 정수형으로 변환
        short newValue = (short)(((im->content[i] - minVal) * normCoef) + 0.5f);
        im->content[i] = newValue;
    }

}

//SetPixel -> 특정 위치에 뭔가를 그리는 함수.
//cmd 창에 이미지 그리는 함수.
void DrawImage(Image im) {
    hwnd = GetForegroundWindow();
    hdc = GetWindowDC(hwnd);
    //Y축 -> X축 순으로 이중 FOR문을 통해 그려짐.
    //윗줄부터 아랫줄 순서로 내려오면서(y 루프), 각 줄은 왼쪽에서 오른쪽으로 픽셀을 그리게 됨(x루프).
    for (int y = 0; y < im->rows; ++y) {
        for (int x = 0; x < im->cols; x++) {
            short pixelValue = im->content[y * im->cols + x];

            // 점을 찍음.
            // 250+x 이것은 대충 +250 위치에서 x, y를 시작해라 라는 의미라고 보면 됨.
            SetPixel(hdc, 250 + x, 250 + y, RGB(
                im->content[y * im->cols + x],
                im->content[y * im->cols + x],
                im->content[y * im->cols + x]));
        }
    }
}

// 이름 그대로 히스토그램 그리기 위한 함수.
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
    // 이 소스코드는 실행 시 Debug 옆의 x64를 x86으로 바꿔야 함.
    // 이미지에 null 포인터를 준 것. 이미지가 포인터 변수이니까, 값을 0으로 줘서 아무것도 가르키고 있지 않다. 라는 의미로 준 것
    Image img = 0;

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            system("cls"); // 기존 콘솔 내용 지우기 위함
            char imgPath[] = "./pgm/no0-1.pgm";
            imgPath[8] = '0' + i;
            imgPath[10] = '1' + j;
            img = readPBMImage(imgPath);
            DrawImage(img);
            DrawHistogram(img);
            imageRelease(img); // 루프 끝날 때마다 해제해줘서, 메모리 누수를 막는다고 함.
            Sleep(10000);
        }
    }
    
    return 0;
}