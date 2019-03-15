def f (x,y):
    return x+y


def main ():
    a=1
    b=2
    return f(a,b)


if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)
