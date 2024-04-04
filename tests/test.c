#include <stdio.h>

void println(int x){
    printf("%d\n", x);
}

int main() {
    if (1 == 1) if (2 == 3) println(1);
    else println(3);
}