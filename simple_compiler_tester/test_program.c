//multiple functions declared whic call each other
// int f() {
//     return 1;
// }

// int g() {
//     return 3+f();
// }

// int main () {
//     return 3*g();

// int g() {
//     return 3+f();
// }

// int main () {
//     return 3*g();
// }

// void f() {
//     int e = 5;
// }

// int main () {
//     int c = 3;
    
//     return c;
// }

// int main () {
//     int b = 2;
//     int a = b;
//     return a;
// }

int main()
{
    int a[5];
    a[2 + 2] = 4;
    return a[4];
}

// int main()
// {
//     int a = 0;
//     int b[5];
//     while (a < 5) {
//         b[a] = 1 + a;
//         a++;
//     }
//     return b[4];
// }