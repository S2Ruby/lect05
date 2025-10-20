#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <unistd.h>     // fork, pipe, read, write
#include <sys/wait.h>   // wait
#define N 4

// 기존 함수 그대로 사용
void sinx_taylor(int num_elements, int terms, double* x, double* result)
{
    for(int i=0; i<num_elements; i++) {
        double value = x[i];
        double numer = x[i] * x[i] * x[i];
        double denom = 6.; // 3!
        int sign = -1;
        for(int j=1; j<=terms; j++) {
            value += (double)sign * numer / denom;
            numer *= x[i] * x[i];
            denom *= (2.*(double)j+2.) * (2.*(double)j+3.);
            sign *= -1;
        }
        result[i] = value;
    }
}

int main()
{
    double x[N] = {0, M_PI/6., M_PI/3., 0.134};
    double res[N];
    int fd[N][2];   // 파이프 N개 (각 자식 전용)

    // 각 x[i]에 대해 자식 생성
    for(int i=0; i<N; i++) {
        pipe(fd[i]);          // 파이프 생성
        pid_t pid = fork();   // 자식 생성

        if(pid == 0) { // 자식 프로세스
            close(fd[i][0]); // 읽기 닫기

            double result;
            sinx_taylor(1, 3, &x[i], &result); // 한 개만 계산
            write(fd[i][1], &result, sizeof(result));

            close(fd[i][1]);
            return 0;         // 자식 종료
        }
        else if(pid > 0) { // 부모 프로세스
            close(fd[i][1]); // 쓰기 닫기
        }
    }

    // 부모가 자식 결과 읽기
    for(int i=0; i<N; i++) {
        read(fd[i][0], &res[i], sizeof(res[i]));
        close(fd[i][0]);
        wait(NULL); // 자식 종료 대기
    }

    // 결과 출력
    for(int i=0; i<N; i++) {
        printf("sin(%.2f) by Taylor series = %f\n", x[i], res[i]);
        printf("sin(%.2f) = %f\n", x[i], sin(x[i]));
    }

    return 0;
}
