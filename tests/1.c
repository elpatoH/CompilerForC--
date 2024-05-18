/* codegen: recursion */
int factorial(int n) {
    if (n <= 0) {
        return 1;
    }
    else {
        return n * factorial(n-1);
    }
}

int main() {
    int n;
    int i, x;

    n = 7;
    
    i = 0;
    while (i <= n) {
        x = factorial(i);
	println(x);
	i = i + 1;
    }
}