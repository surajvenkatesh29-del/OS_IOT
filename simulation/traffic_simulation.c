#include <stdio.h>

#define N 4

int priority(int traffic) {
    if (traffic >= 7) return 1;
    if (traffic >= 4) return 2;
    if (traffic > 0) return 3;
    return 4;
}

int choose(int p[], int last) {
    int best = -1, bp = 5;
    for (int k = 1; k <= N; k++) {
        int i = (last + k) % N;
        if (p[i] < bp) { bp = p[i]; best = i; }
    }
    return best;
}

int main(void) {
    int traffic[N], p[N], last = -1;
    printf("Smart Traffic Signal - PC Simulation\n");
    printf("Enter vehicle/presence level for roads 1-4 (0-10). Ctrl+C to stop.\n\n");

    while (1) {
        printf("Traffic: ");
        for (int i = 0; i < N; i++) {
            if (scanf("%d", &traffic[i]) != 1) return 0;
            if (traffic[i] < 0) traffic[i] = 0;
            if (traffic[i] > 10) traffic[i] = 10;
            p[i] = priority(traffic[i]);
        }

        int selected = choose(p, last);
        if (selected < 0) {
            printf("No traffic detected. Scheduler waits.\n\n");
            continue;
        }
        last = selected;
        printf("Road priorities: R1=%d R2=%d R3=%d R4=%d\n", p[0], p[1], p[2], p[3]);
        printf("Scheduler selected Road %d -> GREEN\n", selected + 1);
        printf("Road %d -> YELLOW -> RED\n\n", selected + 1);
    }
}
