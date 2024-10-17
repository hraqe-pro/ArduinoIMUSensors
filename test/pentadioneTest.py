import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.animation import FuncAnimation
from noise import snoise3

class Pentadion:
    def __init__(self):
        # Początkowy wektor w pięciowymiarowej przestrzeni (x, y, z, t, w)
        self.vector = np.array([1.0, 0.0, 0.0, 0.0, 1.0])  # Ostatnia składowa to dodatkowy wymiar
        self.time = 0.0  # Czas dla szumu Perlin'a

    def rotate(self):
        """
        Aktualizuje orientację pentadionu na podstawie szumu Perlin'a.
        """
        self.time += 0.1  # Zwiększ czas dla szumu Perlin'a
        
        # Ustalanie kątów rotacji na podstawie szumu Perlin'a
        angle_x = snoise3(self.time * 0.1, 0, 0) * np.pi / 4  # Rotacja wokół osi X
        angle_y = snoise3(0, self.time * 0.1, 0) * np.pi / 4  # Rotacja wokół osi Y
        angle_z = snoise3(0, 0, self.time * 0.1) * np.pi / 4  # Rotacja wokół osi Z
        angle_t = snoise3(0, 0, 0) * np.pi / 4  # Rotacja wokół osi czasu
        angle_w = self.time * 0.1  # Rotacja wokół dodatkowej osi
        
        # Rotacje w pięciowymiarowej przestrzeni
        # Macierz rotacji wokół osi X
        rotation_matrix_x = np.array([
            [1, 0, 0, 0, 0],
            [0, np.cos(angle_x), -np.sin(angle_x), 0, 0],
            [0, np.sin(angle_x), np.cos(angle_x), 0, 0],
            [0, 0, 0, 1, 0],
            [0, 0, 0, 0, 1]
        ])
        
        # Macierz rotacji wokół osi Y
        rotation_matrix_y = np.array([
            [np.cos(angle_y), 0, np.sin(angle_y), 0, 0],
            [0, 1, 0, 0, 0],
            [-np.sin(angle_y), 0, np.cos(angle_y), 0, 0],
            [0, 0, 0, 1, 0],
            [0, 0, 0, 0, 1]
        ])
        
        # Macierz rotacji wokół osi Z
        rotation_matrix_z = np.array([
            [np.cos(angle_z), -np.sin(angle_z), 0, 0, 0],
            [np.sin(angle_z), np.cos(angle_z), 0, 0, 0],
            [0, 0, 1, 0, 0],
            [0, 0, 0, 1, 0],
            [0, 0, 0, 0, 1]
        ])
        
        # Macierz rotacji wokół osi czasu (t)
        rotation_matrix_t = np.array([
            [np.cos(angle_t), 0, 0, -np.sin(angle_t), 0],
            [0, 1, 0, 0, 0],
            [0, 0, 1, 0, 0],
            [np.sin(angle_t), 0, 0, np.cos(angle_t), 0],
            [0, 0, 0, 0, 1]
        ])
        
        # Macierz rotacji wokół osi w
        rotation_matrix_w = np.array([
            [1, 0, 0, 0, 0],
            [0, np.cos(angle_w), 0, 0, -np.sin(angle_w)],
            [0, 0, 1, 0, 0],
            [0, 0, 0, 1, 0],
            [0, np.sin(angle_w), 0, 0, np.cos(angle_w)]
        ])

        # Łączenie macierzy rotacji
        self.vector = np.dot(rotation_matrix_w,
                      np.dot(rotation_matrix_t,
                      np.dot(rotation_matrix_z,
                      np.dot(rotation_matrix_y,
                      np.dot(rotation_matrix_x, self.vector)))))

    def to_vector(self):
        """
        Zwraca aktualny wektor pentadionu.
        """
        return self.vector

# Parametry symulacji
steps = 1000  # Ilość kroków symulacji

# Inicjalizacja pentadionu
pentadion = Pentadion()  

# Tablice do przechowywania wyników
trajectory = []

# Tworzenie wykresu 3D
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Inicjalizacja linii do rysowania trajektorii
line, = ax.plot([], [], [], label='Trajektoria rotacji Pentadionu', color='b')

# Ustawienia osi wykresu
ax.set_xlim(-2, 2)
ax.set_ylim(-2, 2)
ax.set_zlim(-2, 2)
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')
ax.set_title('Trajektoria rotacji Pentadionu wokół punktu (0, 0, 0)')
ax.legend()

# Funkcja aktualizująca wykres dla animacji
def update_plot(num):
    pentadion.rotate()  # Aktualizacja orientacji pentadionu
    trajectory.append(pentadion.to_vector()[:3])  # Zapisanie pozycji do trajektorii (tylko x, y, z)
    trajectory_np = np.array(trajectory)  # Konwersja do formatu numpy
    
    # Aktualizacja danych linii
    line.set_data(trajectory_np[:, 0], trajectory_np[:, 1])
    line.set_3d_properties(trajectory_np[:, 2])
    return line,

# Tworzenie animacji
ani = FuncAnimation(fig, update_plot, frames=steps, interval=100, blit=True)

# Wyświetlanie wykresu z animacją
plt.show()
