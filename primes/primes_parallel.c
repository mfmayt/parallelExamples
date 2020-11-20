#include <stdio.h>
#include <tgmath.h>
#include <omp.h>
#include <string.h>

#define N 10000000      // MAX value and size of prime array. you can increase it.

char scTypes[4][20] = {"none","static", "dynamic", "guided"}; // is used for printing

int prime[N] ;
void printPrimes(){
    FILE *fptr = fopen("all_primes.txt","w");
    for(int i=0;i<N;i++){
        int p=prime[i];
        if(p!=0) {
                fprintf(fptr,"%d\n", p);
        }
    }
}
void findNextPrime(int j, int finalIndex){
    int contr = 1;
    for (int k = 1 ;  ; k++) {

        int quo = j / prime[k];
        int rem = j % prime[k];

        if (rem == 0) {
            contr = 0;
            break;
        }else if(quo <= prime[k] || k == finalIndex){
            // We also check if k==finalIndex.Because,for numbers higher than sqrt(N)*[sqrt(N)+1], the quo<=prime[k] will never be true.
            // If k==finalIndex, we know that j was not dividable by any prime number less than or equal to sqrt(N) and that is enough to be sure that j is prime.
            break;
        }

    }

    // If "contr" equals 1 here,we know that the "rem" was never zero.That means that j is prime.
    // We save j in index j of "prime" array.This way, we make sure that parallel threads does not try to write to same index of array,
    // since no thread will use a "j" value equal to "j" value of another thread.
    if(contr==1){
        prime[j]=j;
    }
}
int main() {
    int j;
    int k;
    int n;
    int quo, rem;
    
    FILE *fptr = fopen("output.csv","w");

    fprintf(fptr,"M,Openmp Loop Scheduling Method,Chunck Size, T1,T2,T4,T8,S1,S2,S3\n");    // head of the csv file

    for(int M = 1000 ; M < N + 1 ; M *= 10){              // increasing M number, multiply by 10
        for(int chunkSize = 1 ; chunkSize < 17 ; chunkSize *= 2){       // changes chunk size to 1-2-4-8-16
            for(int schedulingType = 1 ; schedulingType < 4 ; schedulingType++){    // iterates scheduling methods
                fprintf(fptr,"%d,%s,%d,",M, scTypes[schedulingType], chunkSize);
                double threadTimes[9];
                
                for(int threadNum = 1 ; threadNum < 9 ; threadNum *= 2){
                    memset(prime, 0, sizeof prime);
                    double t2 = omp_get_wtime();
                    double t3;
                    prime[0] = 2;
                    n = 3;
                    int finalIndex=0;
                    for (j = 1;; j++) {
                        prime[j] = n;
                        finalIndex=j;
                        int contr = 0;
                        while (contr == 0) {
                            contr=1;
                            n = n + 2;
                            if (n > sqrt(M) ) break;
                            for (k = 1; k <= j; k++) {

                                quo = n / prime[k];
                                rem = n % prime[k];
                                if (rem == 0) {
                                    contr = 0;
                                }
                            }
                        }
                        if (n > sqrt(M) ) break;
                    }

                    // The for loop above is the sequential part where the primes until sqrt(N) are calculated.
                    // Below,the remaining primes will be calculated by checking if a number is dividable by a prime we had found above.
                    // j is the number that we check for primeness
                    switch (schedulingType){ // this switch case statement selects the appropriate scheduling method
                    case 1:
                        #pragma omp parallel  num_threads(threadNum)
                        #pragma omp for schedule(static, chunkSize) nowait
                        for (j = n; j < M ; j+=2) {
                            findNextPrime(j, finalIndex);
                        }
                        break;
                    case 2:
                        #pragma omp parallel  num_threads(threadNum)
                        #pragma omp for schedule(dynamic, chunkSize) nowait
                        for (j = n; j < M ; j+=2) {
                            findNextPrime(j, finalIndex);
                        }
                        break;
                    case 3:
                        #pragma omp parallel  num_threads(threadNum)
                        #pragma omp for schedule(guided, chunkSize) nowait
                        for (j = n; j < M ; j+=2) {
                            findNextPrime(j, finalIndex);
                        }
                        break;
                    }

                    t3 = omp_get_wtime();
                    fprintf(fptr,"%f,", t3 - t2);
                    threadTimes[threadNum] = t3-t2;
                }

                // calculate speedups
                double s1 = threadTimes[2]/threadTimes[2];
                double s2 = threadTimes[4]/threadTimes[2];
                double s3 = threadTimes[8]/threadTimes[4];
                fprintf(fptr,"%f,%f,%f\n",s1,s2,s3);
            }
        }
    }

    printPrimes();
}
