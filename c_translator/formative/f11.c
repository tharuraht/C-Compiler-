int f(int x)
{
  if(x>1){
    return x*f(x-1);
  }else{
    return x;
  }
}

int main()
{
    int a = 5;
    return f(a);
}
