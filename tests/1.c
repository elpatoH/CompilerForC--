/* codegen: recursion */

int main() {
    int x;
    x = 100;
    if (x * x / x < 10 * x / x){
        x = 9;
    }
    println(x);
}