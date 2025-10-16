import matplotlib.pyplot as plt
import numpy as np

threads = [1, 2, 4, 8, 16, 32, 64]
times = [6.65, 3.6, 2.27, 1.2, 0.89, 0.5, 0.49]
T1 = times[0]

speedup = [T1 / t for t in times]
efficiency = [s / n for s, n in zip(speedup, threads)]

plt.figure(figsize=(10, 5))
plt.plot(threads, speedup, marker='o', label='Ускорение (S = T1 / Tn)')
plt.xlabel('Количество потоков')
plt.ylabel('Ускорение')
plt.title('Зависимость ускорения от количества потоков (Массив 10 млн элементов)')
plt.grid(True)
plt.legend()

plt.xticks(threads, labels=[1, 2, 4, 8, 16, 32, 64])

plt.savefig('speedup_10млн.png')
plt.show()

plt.figure(figsize=(10, 5))
plt.plot(threads, efficiency, marker='o', color='red', label='Эффективность (E = S / n)')
plt.xlabel('Количество потоков')
plt.ylabel('Эффективность')
plt.title('Зависимость эффективности от количества потоков (Массив 10 млн элементов)')
plt.grid(True)
plt.legend()

plt.xticks(threads, labels=[1, 2, 4, 8, 16, 32, 64])

plt.savefig('efficiency_10млн.png')
plt.show()