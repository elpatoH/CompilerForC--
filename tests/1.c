/* codegen: expressions containing at most one operator */

/* ---REMOVE TO COMPILE AS C CODE---
#include <stdio.h>
void println(int x) { printf("%d\n", x); }
---REMOVE TO COMPILE AS C CODE--- */

int main() {
    int x;

    //FIX THIS neg at start dissapears
    x = -123 - 123;
    println(x);

}