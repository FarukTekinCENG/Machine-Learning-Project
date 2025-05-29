import matplotlib.pyplot as plt
import numpy as np

n = 5  # Toplam dosya sayısı

# Her bir dosyadan verileri oku ve ayrı bir grafik oluştur
for i in range(1, n+1):
    gd_filename = f'gdEpochSureLossData{i}.txt'
    sgd_filename = f'sgdEpochSureLossData{i}.txt'
    adam_filename = f'adamEpochSureLossData{i}.txt'
    
    try:
        # GD için verileri dosyadan oku
        gd_data = np.loadtxt(gd_filename)
        gd_epoch = gd_data[:, 0]
        gd_elapsed_time = gd_data[:, 1]
        gd_error = gd_data[:, 2]

        # SGD için verileri dosyadan oku
        sgd_data = np.loadtxt(sgd_filename)
        sgd_epoch = sgd_data[:, 0]
        sgd_elapsed_time = sgd_data[:, 1]
        sgd_error = sgd_data[:, 2]

        # Adam için verileri dosyadan oku
        adam_data = np.loadtxt(adam_filename)
        adam_epoch = adam_data[:, 0]
        adam_elapsed_time = adam_data[:, 1]
        adam_error = adam_data[:, 2]

        # Süre-loss grafiği
        plt.figure(figsize=(15, 5))
        
        plt.subplot(1, 2, 1)
        plt.plot(gd_elapsed_time, gd_error, marker='o', label='GD')
        plt.title(f'Süre-Loss Grafiği - Dosya {i}')
        plt.xlabel('Geçen Süre (saniye)')
        plt.ylabel('Loss Değeri')
        plt.legend()

        plt.subplot(1, 2, 2)
        plt.plot(sgd_elapsed_time, sgd_error, marker='o', label='SGD')
        plt.title(f'Süre-Loss Grafiği - Dosya {i}')
        plt.xlabel('Geçen Süre (saniye)')
        plt.ylabel('Loss Değeri')
        plt.legend()

        plt.show()

        plt.figure(figsize=(15, 5))

        plt.subplot(1, 2, 1)
        plt.plot(adam_elapsed_time, adam_error, marker='o', label='Adam')
        plt.title(f'Süre-Loss Grafiği - Dosya {i}')
        plt.xlabel('Geçen Süre (saniye)')
        plt.ylabel('Loss Değeri')
        plt.legend()

        plt.subplot(1, 2, 2)
        plt.plot(gd_epoch, gd_error, marker='o', label='GD')
        plt.title(f'Epoch-Loss Grafiği - Dosya {i}')
        plt.xlabel('Epoch')
        plt.ylabel('Loss Değeri')
        plt.legend()

        plt.show()

        plt.figure(figsize=(15, 5))

        plt.subplot(1, 2, 1)
        plt.plot(sgd_epoch, sgd_error, marker='o', label='SGD')
        plt.title(f'Epoch-Loss Grafiği - Dosya {i}')
        plt.xlabel('Epoch')
        plt.ylabel('Loss Değeri')
        plt.legend()

        plt.subplot(1, 2, 2)
        plt.plot(adam_epoch, adam_error, marker='o', label='Adam')
        plt.title(f'Epoch-Loss Grafiği - Dosya {i}')
        plt.xlabel('Epoch')
        plt.ylabel('Loss Değeri')
        plt.legend()

        plt.show()

    except FileNotFoundError:
        print(f"Bir veya daha fazla dosya bulunamadı.")
