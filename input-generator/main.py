import sys
import random

def generate(n_groups, min_points, max_points, k):
    with open("example.in", "w") as f:
        groups = int(n_groups)

        f.write(f"n_groups={groups}\n")
        
        for i in range(groups):
            label = chr(65 + i if (i < 26) else 97 + (i % 26))
            length = random.randint(int(min_points), int(max_points))

            f.write(f"label={label}\n")
            f.write(f"length={length}\n")

            muX = random.randint(groups * -2, groups * 2)
            muY = random.randint(groups * -2, groups * 2)

            for _ in range(length):
                x = random.normalvariate(muX, 1)
                y = random.normalvariate(muY, 1)
                f.write(f"({x:.2f},{y:.2f})\n")
                

        f.write(f"k={k}\n")

        tbdX = random.normalvariate(0, 1)
        tbdY = random.normalvariate(0, 1)
        f.write(f"({tbdX:.2f},{tbdY:.2f})")

def main():
    if (len(sys.argv) < 5):
        print("Usage: python main.py <n_groups> <min_points_per_group> <max_points_per_group> <k>")
        print("{:>20} - {:>20}".format("n_groups", "Number of groups (up to 52)"))
        print("{:>20} - {:>20}".format("min_points_per_group", "Minimum number of points per group"))
        print("{:>20} - {:>20}".format("max_points_per_group", "Maximum number of points per group"))
        print("{:>20} - {:>20}".format("k", "k parameter to be used in the algorithm"))

        print("\n\nThis script will generate the input and save it to a file called \"example.in\".")
        return

    generate(*sys.argv[1:])

if (__name__ == '__main__'):
    main()