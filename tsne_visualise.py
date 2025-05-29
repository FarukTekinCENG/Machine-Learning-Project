import numpy as np
from sklearn.manifold import TSNE
import matplotlib.pyplot as plt
import os

# Veriyi okuma
def read_data(file_path):
    data = []
    with open(file_path, 'r') as file:
        for line in file:
            # Satırdaki double sayıları ayır ve listeye dönüştür
            values = list(map(float, line.split()))
            data.append(values)
    return np.array(data)

# T-SNE modelini oluştur
tsne = TSNE(n_components=2, random_state=42)

# Tüm dosyaları oku ve renklerle birlikte birleştir
combined_data = []
colors = []

directory_path = 'data/'  # Veri dosyalarının bulunduğu klasörün adını ve yolunu belirtin
for i, filename in enumerate(os.listdir(directory_path)):
    if filename.endswith(".txt"):
        file_path = os.path.join(directory_path, filename)
        
        # Veriyi oku
        data = read_data(file_path)
        combined_data.append(data)
        
        # Renk oluştur
        color = plt.cm.jet(i / len(os.listdir(directory_path)))  # Dosya sayısına göre renk oluştur
        colors.extend([color] * len(data))

# Noktaları birleştir
combined_data = np.concatenate(combined_data, axis=0)

# Renkler dizisini oluştur (her bir dosya için bir renk)
colors = np.array(colors)[:len(combined_data)]

# T-SNE ile boyut indirgeme
result = tsne.fit_transform(combined_data)

# Sonuçları görselleştir
plt.scatter(result[:, 0], result[:, 1], c=colors)
plt.title('T-SNE Görselleştirme')
plt.show()
