import matplotlib.pyplot as plt

if __name__ == "__main__":
    x = []
    y = []
    for N in range(1, 10000):
        x.append(N)
        temp = pow(1-float(1.0/N), N-1)
        y.append(temp)
    plt.figure()
    plt.xlabel('number of nodes(N')
    plt.ylabel('channel utilization')
    plt.semilogx(x, y)
    plt.show()