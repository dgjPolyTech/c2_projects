#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h> // malloc, free, exit 함수 사용을 위해 추가
#include <string.h> // strcmp 함수 사용을 위해 추가

#include <Windows.h> // cmd창 위에 요소를 그리기 위해 필요한 헤더
#include <math.h>

HWND hwnd; // 콘솔 핸들. 대충 콘솔 창에 띄우기 위한 콘솔의 값을 가진 것이라고 볼 수 있음.
HDC hdc; // cmd 위에 그림을 그릴 각종 기능들을 포함함.

// 이미지 유형(포맷) 모음
// enum은 1, 2, 3, 4, 5, 6 이런 식으로 "열거형"으로 데이터를 저장하기 위해 사용함.
enum FORMAT { EMPTY, GREY, RGB, YCBCR, YCBCR420, BLOCK };

// 이미지 정보를 담는 구조체
typedef struct imageType {
    unsigned int rows;      // 이미지 세로 크기(=높이)
    unsigned int cols;      // 이미지 가로 크기(=넓이)
    char format;            // 위에서 정의한 이미지 포맷 정보.
    unsigned long total;    // 전체 픽셀 요소 개수
    unsigned int levels;    // 최대 밝기 값.
    short* content;         // 실제 픽셀 데이터가 저장되는 포인터
} ImageType;

typedef ImageType* Image;

// 가로 세로 평균/분산 값 구하는 함수
void image_avg(Image im, float* mean_x, float* mean_y, float* var_x, float* var_y) {
    // 빈 이미지는 무효로 처리
    if (im == NULL || im->content == NULL) {
        *mean_x = *mean_y = *var_x = *var_y = 0;
        return;
    }

    short* horizontal = malloc(sizeof(short) * im->cols); // 가로 전체 픽셀 수를 담을 "공간" 생성.
    short* vertical = malloc(sizeof(short) * im->rows); // 세로 전체 픽셀 수를 담을 공간 생성
    long long dark_pixel_count = 0; // 이미지 데이터를 보고 어두운 총 픽셀 수를 계산함. 

    // horizontal/vertical의 초기값 모두 0으로 만들어두기
    for (unsigned int i = 0; i < im->cols; i++) horizontal[i] = 0;
    for (unsigned int i = 0; i < im->rows; i++) vertical[i] = 0;

    // y 축을 기준으로, 가로로 이동하면서 픽셀 순회
    for (unsigned int y = 0; y < im->rows; y++) {
        for (unsigned int x = 0; x < im->cols; x++) {
            // 일정 밝기값 이하일 경우(=색칠되었다는 의미) horizon/vertical/dark_pixel_count 3개 모두 증가
            if (im->content[y * im->cols + x] < 10) {
                horizontal[x]++;      
                vertical[y]++;       
                dark_pixel_count++;
            }
        }
    }
    // 만약 어두운 픽셀이 없으면(=아무것도 색칠이 되지 않았다는 의미) 함수 종료
    if (dark_pixel_count == 0) {
        *mean_x = *mean_y = *var_x = *var_y = 0;
        free(horizontal);
        free(vertical);
        return;
    }

    long long sum_x = 0;
    // x축 기준 평균 구하는 계산 식: (long long)x(현재 x 좌표) 곱하기 horizontal[x](그 좌표 상의 어두운 픽셀의 개수)(실질적으로는 세로로 쭉~세는 느낌)
    for (unsigned int x = 0; x < im->cols; x++) {
        sum_x += (long long)x * horizontal[x];
    }

    // 위에서 구한 값을, 전체 어두운 픽셀 수 만큼 나눔
    *mean_x = (float)sum_x / dark_pixel_count;

    long long sum_y = 0;
    for (unsigned int y = 0; y < im->rows; y++) {
        sum_y += (long long)y * vertical[y];
    }

    *mean_y = (float)sum_y / dark_pixel_count;

    double sum_var_x = 0; 
    for (unsigned int x = 0; x < im->cols; x++) {
        sum_var_x += pow(x - *mean_x, 2) * horizontal[x];
    }

    *var_x = (float)(sum_var_x / dark_pixel_count);

    double sum_var_y = 0;

    for (unsigned int y = 0; y < im->rows; y++) {
        sum_var_y += pow(y - *mean_y, 2) * vertical[y];
    }

    *var_y = (float)(sum_var_y / dark_pixel_count);

    free(horizontal);
    free(vertical);
}


// 위에서 선언한 구조체 기반으로, 이미지 저장 공간을 만들어내는 역할의 함수.
Image imageAllocate(unsigned int rows, unsigned int cols, char format, unsigned int levels) { //unsigned int > 음수를 표현할 수 없는 대신 양수로 표현할 수 있는 범위가 훨씬 더 큼
    // 이미지의 크기를 malloc 형식으로 필요한 만큼만 동적 할당. 
    Image im = malloc(sizeof(ImageType));
    im->rows = rows;
    im->cols = cols;
    im->format = format;
    im->levels = levels;
    im->content = NULL;
    if (im->format == EMPTY)
        return im;

    // 이미지 형식에 따라 total 값(=이미지의 전체 픽셀 개수)을 필요한 만큼 할당
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

// 사용하던 메모리를 해제하는 함수
void imageRelease(Image im) {
    im->format = EMPTY;
    if (im->content != NULL)
        free(im->content);
    im->content = NULL;
    free(im);
}

// PBM 이미지를 읽는 구간.
Image readPBMImage(const char* filename) {
    FILE* pgmFile;

    char signature[3];
    unsigned cols, rows, levels; // 

    Image im = imageAllocate(0, 0, EMPTY, 0); // 먼저 비어있는 껍데기 이미지를 만들어냄. 이미지가 아예 없어 오류가 나는 상황을 막기 위함.

    pgmFile = fopen(filename, "rb"); // read binary 모드로 파일을 엶
    if (pgmFile == NULL) {
        perror("Cannot open file to read!");
        return im;
    }
    // signature, = 이미지의 매직 넘버(대충 이미지 유형)을 읽어들이고, sizeof(signature), = 읽어올 데이터의 "최대 크기"는 signature 변수의 크기만큼으로 제한한다. pgmFile= 그것을 pgmFile을 토대로 한다.
    fgets(signature, sizeof(signature), pgmFile);

    // P5, P6, P4가 아니면 오류 발생
    if (strcmp(signature, "P5") != 0 && strcmp(signature, "P6") != 0 && strcmp(signature, "P4") != 0) {
        perror("Wrong file type!");
        fclose(pgmFile);
        return im;
    }

    // pgmFile로부터 행, 열, 최대 밝기값을 긁어온다.
    // %d %d %d 형식으로 데이터를 읽어오면, 공백 등에 상관없이 각각 가장 먼저 만나는 정수값을 가져오게 된다.
    // pgm 파일의 헤더는 처음 열면 순서대로 매직 넘버(정수 아님)/열 개수(너비)/행 개수(높이)/최대 밝기 값 이렇게 정리되어 있기에 가져올 수 있는 것이다.
    // 최대 밝기값은, 최대 밝기값을 기준으로 이미지의 밝기를 정해야 하기에 가져오는 것이다.
    fscanf(pgmFile, "%d %d %d", &cols, &rows, &levels);
    fgetc(pgmFile); // 헤더의 마지막 줄바꿈 문자 제거. 정확히는 헤더까지만 읽고, 뒤에 줄바꿈 문자 이후부터는 읽을 필요가 없으니 이렇게 버리는 것

    // 기존의 빈 이미지를 해제하고 실제 크기에 맞게 다시 할당
    imageRelease(im);

    // 이미지 유형에 따라, 이미지 객체를 생성하는 구간이다.
    if (strcmp(signature, "P5") == 0 || strcmp(signature, "P4") == 0) {// 이 부분은 매직 넘버가 P5 혹은 P4 "이면" 조건을 수행한다는 의미이다.
        // C에서는 0을 False로 쓰지만, strcmp의 경우 두 문자열의 차이를 반환하므로 0일 경우 True(같다)이다.
        im = imageAllocate(rows, cols, GREY, levels);
        for (unsigned int k = 0; k < im->total; ++k) {
            im->content[k] = (unsigned char)fgetc(pgmFile);
        }
    }
    else if (strcmp(signature, "P6") == 0) {
        im = imageAllocate(rows, cols, RGB, levels);
        unsigned long gOffset = im->cols * im->rows;
        unsigned long bOffset = 2 * im->cols * im->rows;
        // RGB 유형의 경우, 이름 그대로 RGB 값으로 데이터가 저장되어 있다. 메모리에는 R채널, G채널, B채널을 각각 따로 저장한다.
        //gOffset은 G채널 데이터가 시작될 위치, bOffset은 B채널 데이터가 시작될 위치를 가리킨다.
        for (unsigned int k = 0; k < im->total / 3; ++k) {
            im->content[k] = (unsigned char)fgetc(pgmFile);
            im->content[k + gOffset] = (unsigned char)fgetc(pgmFile);
            im->content[k + bOffset] = (unsigned char)fgetc(pgmFile);
        }
    }

    fclose(pgmFile);
    return im;
}

// PBM 이미지 쓰기
void writePBMImage(const char* filename, const Image im) {
    FILE* pgmFile;

    pgmFile = fopen(filename, "wb"); // 파일 이름을 받아와, 바이너리 쓰기(write/binary) 모드로 연다.
    if (pgmFile == NULL) {
        perror("Cannot open file to write");
        exit(-1);
    }

    // 흑백은 GREY, RGB는 P6로 매직넘버를 적는다.
    if (im->format == GREY)
        fprintf(pgmFile, "P5\n");
    else if (im->format == RGB)
        fprintf(pgmFile, "P6\n");
    else {
        perror("Unknown file format\n");
        fclose(pgmFile);
        return;
    }

    // 헤더의 남은 부분인 행 개수(높이), 열 개수(너비), 최대 밝기 적는다.
    fprintf(pgmFile, "%d %d\n", im->cols, im->rows);
    fprintf(pgmFile, "%d\n", im->levels);

    // 흑백일 경우, TOTAL(=이미지 픽셀 요소 개수)만큼 반복하며 점을 찍는 식으로 이미지 구성한다.
    if (im->format == GREY) {
        for (unsigned int k = 0; k < im->total; ++k)
            fputc((unsigned char)im->content[k], pgmFile);
    }
    //RGB의 경우, readPBM에서 했던 과정의 역순이라 이해하면 쉽다. 
    else if (im->format == RGB) {
        //R은 시작점 자체가 R만 모여있어서 따로 구역을 안나눴고, G와 B만 각각 gOffset/bOffset으로 나눈 것이다.
        unsigned long gOffset = im->cols * im->rows;
        unsigned long bOffset = 2 * im->cols * im->rows; // 2*가 붙어있는 이유는, r->G->B 순서대로 3번째에 B 데이터가 오기 때문이다.
        // 즉, 그릴 때 저만큼 떨어진 "위치에" 있는 값을 가져와 넣는 것이 offset의 의미라고 볼 수 있다.
        for (unsigned int k = 0; k < im->total / 3; ++k) {
            fputc((unsigned char)im->content[k], pgmFile);
            fputc((unsigned char)im->content[k + gOffset], pgmFile);
            fputc((unsigned char)im->content[k + bOffset], pgmFile);
        }
    }

    fclose(pgmFile); // 파일 다 썼으니 닫기
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
    for (unsigned int y = 0; y < im->rows; ++y) {
        for (unsigned int x = 0; x < im->cols; x++) {
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
    // 가로/세로 정의
    short* horizontal = malloc(sizeof(short) * im->cols);
    short* vertical = malloc(sizeof(short) * im->rows);

    // 먼저 가로/세로 전체 크기만큼 다 0으로 지정한다.
    for (unsigned int i = 0; i < im->cols; i++)
    {
        horizontal[i] = 0;
    }
    for (unsigned int i = 0; i < im->rows; i++)
    {
        vertical[i] = 0;
    }

    // 세로로 한 줄 씩 아래로 이동한다.
    for (unsigned int y = 0; y < im->rows; ++y)
    {
        // 가로행 픽셀 개수만큼 옆으로 이동한다.
        for (unsigned int x = 0; x < im->cols; x++)
        {
            // 특정 조건에 해당하는 경우? 이 부분을 0이 아닌 1로 채운다.
            if (im->content[y * im->cols + x] < 10)
            {
                horizontal[x]++;
            }
        }
    }

    //가로로 이동하며 옆으로 이동한다.ㅣ
    for (unsigned int x = 0; x < im->cols; ++x)
    {
        // 세로행 픽셀 개수만큼 아래로 이동한다.
        for (unsigned int y = 0; y < im->rows; y++)
        {
            // 역시 특정 조건에 해당하는 경우 1로 변환한다.
            if (im->content[y * im->cols + x] < 10)
            {
                vertical[y]++;
            }
        }
    }

    hwnd = GetForegroundWindow();
    hdc = GetWindowDC(hwnd);
    // 위에서 정의된 위치를 토대로 픽셀을 찍는다.
    for (unsigned int i = 0; i < im->cols; i++)
    {
        for (int j = 0; j < horizontal[i] * 20; j++)
        {
            SetPixel(hdc, 250 + i, 250 - j, RGB(0, 255, 0));
        }
    }
    for (unsigned int i = 0; i < im->rows; i++)
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

    // while(1)을 사용하여 무한 루프 생성
    while (1) {
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                system("cls"); // 기존 콘솔 내용 지우기 위함
                char imgPath[] = "./pgm/no0-1.pgm";
                imgPath[8] = '0' + i;
                imgPath[10] = '1' + j;

                img = readPBMImage(imgPath);

                // 이미지와 히스토그램을 먼저 화면에 그림
                DrawImage(img);
                DrawHistogram(img);

                // 현재 그려진 이미지에 대한 분석 값을 계산하기 위한 변수
                float mx, my, vx, vy;
                image_avg(img, &mx, &my, &vx, &vy);

                // 분석 결과를 일반 printf로 화면에 출력
                printf("\n--- Analysis for %s ---\n", imgPath);
                printf("H-Mean: %.2f, V-Mean: %.2f\n", mx, my);
                printf("H-Var:  %.2f, V-Var:  %.2f\n", vx, vy);

                imageRelease(img); // 루프 끝날 때마다 해제해줘서, 메모리 누수를 막는다고 함.
                Sleep(5000);
            }
        }
    }
    return 0;
}