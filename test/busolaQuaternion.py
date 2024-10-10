import serial
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from scipy.spatial.transform import Rotation as R

# Inicjalizacja portu szeregowego (dopasuj do swojego portu)
ser = serial.Serial('COM6', 115200, timeout=1)

# Wektor pola magnetycznego Ziemi w mikroteslach
B_earth_nT = np.array([19720.3, 2191.0, 45805.4])  # w nano-teslach
B_earth = B_earth_nT / 1000  # konwersja do mikro-tesli (µT)

# Funkcja rysowania wykresu (inicjalizacja)
def init_plot():
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    # Ustawienia osi
    ax.set_xlim([-1, 1])
    ax.set_ylim([-1, 1])
    ax.set_zlim([-1, 1])

    ax.set_xlabel('X (µT)')
    ax.set_ylabel('Y (µT)')
    ax.set_zlabel('Z (µT)')

    return fig, ax

# Aktualizacja wektorów na wykresie
def update_plot(B_magnetometer, B_earth_rotated, quiver_magnetometer, quiver_earth):
    # Aktualizacja wektora magnetometru
    quiver_magnetometer.remove()
    quiver_earth.remove()

    # Rysowanie nowych wektorów
    quiver_magnetometer = ax.quiver(0, 0, 0, B_magnetometer[0], B_magnetometer[1], B_magnetometer[2], color='r', label='Wektor magnetometru', length=1.0, normalize=True)
    quiver_earth = ax.quiver(0, 0, 0, B_earth_rotated[0], B_earth_rotated[1], B_earth_rotated[2], color='b', label='Wektor Ziemi (po obrocie)', length=1.0, normalize=True)

    plt.draw()
    plt.pause(0.01)  # Mała przerwa, by umożliwić odświeżenie wykresu

    return quiver_magnetometer, quiver_earth

def read_serial_data():
    """
    Odczyt danych z portu szeregowego i przetworzenie ich na wektor magnetometru oraz kwaternion
    """
    line = ser.readline().decode('utf-8').strip()
    
    # Sprawdzenie, czy linia zaczyna się od 'Final Quaternion: '
    if line.startswith("Final Quaternion:"):
        try:
            # Usuń etykietę 'Final Quaternion: ' i rozdziel liczby
            data_str = line.replace("Final Quaternion: ", "")
            data = list(map(float, data_str.split(', ')))

            # Dane: [kwaternion_w, kwaternion_x, kwaternion_y, kwaternion_z, mag_x, mag_y, mag_z]
            if len(data) == 7:
                # Kwaternion
                q = [data[1], data[2], data[3], data[0]]  # Kolejność: [x, y, z, w]

                # Wektor magnetometru w µT
                B_magnetometer = np.array([data[4], data[5], data[6]])

                return B_magnetometer, q
        except ValueError:
            print(f"Błąd przy przetwarzaniu danych: {line}")
    return None, None

# Inicjalizacja wykresu
fig, ax = init_plot()

# Inicjalizacja pustych wektorów na wykresie
quiver_magnetometer = ax.quiver(0, 0, 0, 0, 0, 0, color='r', label='Wektor magnetometru', length=1.0, normalize=True)
quiver_earth = ax.quiver(0, 0, 0, 0, 0, 0, color='b', label='Wektor Ziemi (po obrocie)', length=1.0, normalize=True)

# Pętla do ciągłego odczytu danych z Seriala i aktualizowania wykresu
while True:
    B_magnetometer, q = read_serial_data()
    if B_magnetometer is not None and q is not None:
        # Użycie kwaternionu do przekształcenia wektora pola magnetycznego Ziemi
        rotation = R.from_quat(q)  # kwaternion w formacie [x, y, z, w]
        B_earth_rotated = rotation.apply(B_earth)

        # Aktualizacja wykresu
        quiver_magnetometer, quiver_earth = update_plot(B_magnetometer, B_earth_rotated, quiver_magnetometer, quiver_earth)
