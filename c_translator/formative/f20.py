def main():
    a = 5 + (5 * (1 + 1))
    return a

if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)