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
// int a[5];
// int b = 5;

int main()
{
    // int a[5] = {0,1,2,3,4};
    //int x = a[5] + a[3] + a[1];
    int a = 5*3*2;
    int b = 6;
    return a;
    // return x + b;
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

// int main(){

//     int a = 5*3*2;
//     int b = 6;

//     return a;
// }

int main() {
	int x=999;
	int y = 9;
    int z = 2;
   	while(y) {
       		y = y+ x;
        	break;
       		z = 465; /*dead code*/ 
    	}

    	return z;
}

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

