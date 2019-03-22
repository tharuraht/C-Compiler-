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

int f(int x, int y, int z){

x = x-1;
y = y+1;
return x + y + z;
}

int main(){

// int output = f(10,10,10);
int a = 3;
while (a > 0) {
    // int x = 3;
    a = a - 1;
}
return a;

}
