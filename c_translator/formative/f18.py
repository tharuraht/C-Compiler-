def main ():
    a=5
    b=1
    if (a>5):
        if (b==1):
            return a+b
        else:
            return a
    return 10

if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)
