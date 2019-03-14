def main():
    a = 5
    if (a == 5):
        a = 4
    return a

# Boilerplate
if __name__ == "__main__":
    import sys
    ret=main()
    sys.exit(ret)