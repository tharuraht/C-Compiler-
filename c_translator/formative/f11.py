def f(x):
   if x>1:
       return x*f(x-1)
   else:
       return x

def main():
    a = 5
    return f(a)

# Boilerplate
if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)
