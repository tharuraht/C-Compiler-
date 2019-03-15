def main ():
    a=5
    b=1
    if (a > 2 or b==4):
        return a+b
    return a

if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)

