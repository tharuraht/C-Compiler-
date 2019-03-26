// multiple functions declared whic call each other
// int f() {
//     return 1;
// }

// int g() {
//     return 2+f();
// }

// int main () {
// 	int a = 1;
// 	int b = a++;
//     return b;
// }
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

// int a[5];
// int b = 5;
// int m = 10;
// enum a{x,y=9,z};
// int main () {
// 	int b[5];
// 	b[0] = 5;
// 	enum g{j=3,k,l};
// 	return z + b[0] + l;
// }

// int main () {
// 	int y=0;
// 	for (int x=2; x<10; x++) {
// 		if (x==5) {
// 			continue;
// 		}
// 		y = y + (y+x);
// 	}
// 	return y;
// }


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

// int main(){

//     int a = 5*3*2;
//     int b = 6;

//     return a;
// }

// int main() {
// 	int x=999;
// 	int y = 9;
//     int z = 2;
//    	while(y) {
//        		y = y+ x;
//         	break;
//        		z = 465; /*dead code*/ 
//     	}

//     	return z;
// }

// int main()                    
// {     
//      int x=99;
//     int y = 10;
//     for(x = -10; x < 10; x++)
//     {
//         if (x) {
//             y = y+1;    
//             break;
//             y = y-x;
//         }
//         y = y+x;
//     }
//     return y;                                         
// }

// int main () {
//     int a = 0;
//     while (a < 11)
//     {
//         while (a < 12)
//         {
//             if (a > 7)
//                 break;
//             a++;
//         }
//         if (a > 8)
//             break;
//         a++;
//     }
//     return a;
// }

// int main () {
//     int a;
//     int b = 0;
//     for (a = 10; a > 5; a--)
//     {
//         if (a == 7)
//             continue;
//         b = b + a;
//     }
//     for (int i = 0; i < 10; ++i)
//         if (b == 7)
//             break;
//         else
//             b = b + i;

//     return b;
// }

// int a[10];
// int b = 5;
// int main()
// {
//     a[0] = 7;
//     a[5] = 10;
//     int a = 0;
//     b = a;
//     return a[b];
// }

// int main () {
// int a = 5;
// if (a == 4) return 9;
// else if (a>4) 
// return 10;
// }

// int collatz_recursive(int n)
// {
//     if (n == 1)
//     {
//         return n;
//     }

//     if (n % 2 == 0)
//     {
//         return n + collatz_recursive(n / 2);
//     }
//     else
//     {
//         return n + collatz_recursive(n * 3 + 1);
//     }
// }


    int main()
    {
        int a = 1;
        int b = 2;
        int c = 3;
        int d;
        int e;
        int f[2];
        
        while (a < 10)
        {
            int b = 10;

            a++;
            d = b;
            f[0] = 3;
            f[1] = a;
        }
        for (c = 0; c < 10; c++)
        {
            int a = 15;
            int f[2];

            e = c;
            f[0] = 7;
            f[1] = a;
        }

        return a + b + c + d + e + f[0] + f[1];
    }
