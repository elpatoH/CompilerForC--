/* astgen: associativity of * and / operators together
   with parenthesized subexpressions */

int f() {
    int x, y, u, v, w;

    x = x+y * (x/y);
}