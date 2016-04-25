#include <stdlib.h>
#include <pthread.h>
#include <iostream>

using namespace std;

#define N 5           
#define NLOOP 10      

int n;
enum procphase { 
    out_cr, 
    want_cr, 
    claim_cr 
} *procphase;
enum procphase* flags;
int turn;
int counter;

void init_mutex(int nproc)
{
    n = nproc;
    flags = new enum procphase [n];
    srand(time(0));
    turn = (int) (1.0 * n * rand() / (RAND_MAX + 1.0));
    for (int i = 0; i < n; i++)
        flags[i] = out_cr;
}

void mutex_lock(int i) {
    flags[i] = want_cr;
    int j = turn;
    do {
        while (j != i) {
            if (flags[j] == out_cr)
                j = (j + 1) % n;
            else
                j = turn;
        }
        flags[i] = claim_cr;
        j = (j + 1) % n;
        while (flags[j] != claim_cr)
            j = (j + 1) % n;
    } while (!(j == i && (turn == i || flags[turn] == out_cr)));
    turn = i;
}

void mutex_unlock(int i) {
    int j = (turn + 1) % n;
    while (flags[j] == out_cr)
        j = (j + 1) % n;
    turn = j;
    flags[i] = out_cr;
}

void *inc_fun(void *arg) {
    int i, val;
    int tid = *((int*)(&arg));
    for (i = 0; i < NLOOP; i++) {
        mutex_lock(tid);
        val = counter;
        counter = val + 1;
        cout << tid << ": " << counter << endl;
        mutex_unlock(tid);
    }
    return NULL;
}

int main() {
    pthread_t tid[N];
    init_mutex(N);
    int i;
    for (i = 0; i < N; i++) 
        pthread_create(&tid[i], NULL, inc_fun, (void *)i);
    for (i = 0; i < N; i++) 
        pthread_join(tid[i], NULL);

    return 0;
}