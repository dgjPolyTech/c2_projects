#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h> // malloc, free, exit 함수 사용을 위해 추가
#include <string.h> // strcmp 함수 사용을 위해 추가

// 이미지 유형(포맷) 모음
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


int main(void)
{
    // 이 소스코드는 실행 시 Debug 옆의 x64를 x86으로 바꿔야 함.
    Image img = NULL;
    char imgPath[] = "./frog.pbm"; // 원본 이미지 경로 (흑백 PGM 파일이어야 합니다)

    // 원본 이미지 읽기
    img = readPBMImage(imgPath);

    // 이미지를 성공적으로 읽었는지 확인
    if (img != NULL && img->format != EMPTY) {
        // 히스토그램 평활화 적용
        histogramEqualization(img);

        // 결과 이미지를 새 파일로 저장
        const char* outputFilename = "frog_equalized22.pbm";
        writePBMImage(outputFilename, img);

        // 사용한 메모리 해제
        imageRelease(img);
    }
    else {
        printf("이미지를 찾을 수 없습니다..\n");
    }

    return 0;
}