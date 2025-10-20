#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#define N 4

void sinx_taylor(int num_elements, int terms, double* x, double* result)
{
    for(int i=0; i<num_elements; i++) {
        double value = x[i];
        double numer = x[i] * x[i] * x[i];
        double denom = 6.;
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
    int fd[N][2];

    for(int i=0; i<N; i++) {
        pipe(fd[i]);
        pid_t pid = fork();

        if(pid == 0) { 
            close(fd[i][0]);

            double result;
            sinx_taylor(1, 3, &x[i], &result);
            write(fd[i][1], &result, sizeof(result));

            close(fd[i][1]);
            return 0;
        }
        else if(pid > 0) {
            close(fd[i][1]);
        }
    }

    for(int i=0; i<N; i++) {
        read(fd[i][0], &res[i], sizeof(res[i]));
        close(fd[i][0]);
        wait(NULL);
    }

    for(int i=0; i<N; i++) {
        printf("sin(%.2f) by Taylor series = %f\n", x[i], res[i]);
        printf("sin(%.2f) = %f\n", x[i], sin(x[i]));
    }

    return 0;
}
