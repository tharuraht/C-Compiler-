// //multiple functions declared whic call each other
// int f() {
//     return 1;
// }

// int g() {
//     return 3+f();
// }

// int main () {
//     return 3*g();
// }

int main () {
    int c = 3;
    f();
    return c;
}

void f() {
    int c = 5;
}