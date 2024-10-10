import serial
import time
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Ustawienia portu COM
PORT = 'COM6'
BAUDRATE = 115200
DELAY = 0.1  # czas odświeżania wykresu

# Funkcja do odczytu danych z seriala
def read_serial_data(serial_port):
    try:
        line = serial_port.readline().decode('utf-8').strip()  # Odczyt lini danych z seriala
        print(f"Odczytana linia: {line}")  # Dodano wyświetlanie odczytanej linii
        if "CORRECTED:" in line:
            # Odczyt i rozbicie danych na X, Y, Z po prefiksie "CORRECTED:"
            corrected_data = line.split("CORRECTED:")[1].strip()
            data = corrected_data.split(',')
            
            # Sprawdzanie, czy mamy dokładnie 3 wartości (X, Y, Z)
            if len(data) == 3:
                x, y, z = [float(value) for value in data]
                return x, y, z
            else:
                print("Niepoprawny format danych: brak 3 wartości po CORRECTED:")
    except Exception as e:
        print(f"Error reading data: {e}")
    return None, None, None  # Zwracamy None jeśli coś poszło nie tak

# Obliczanie orientacji (yaw, pitch, roll) na podstawie X, Y, Z
def calculate_yaw_pitch_roll(x, y, z):
    # Yaw: obrót wokół osi Z
    yaw = np.arctan2(y, x)

    # Pitch: przechylenie do przodu/tyłu wokół osi Y
    pitch = np.arcsin(-z)

    # Roll: przechylenie w lewo/prawo wokół osi X
    roll = np.arctan2(z, np.sqrt(x**2 + y**2))

    return yaw, pitch, roll

# Rysowanie strzałki w przestrzeni 3D
def plot_arrow(ax, yaw, pitch, roll):
    # Obliczenie wektora orientacji w przestrzeni 3D
    # Używamy yaw, pitch, roll do określenia kierunku wektora.
    direction = np.array([
        np.cos(pitch) * np.cos(yaw),  # X komponent
        np.cos(pitch) * np.sin(yaw),  # Y komponent
        np.sin(pitch)                 # Z komponent
    ])
    
    # Wyczyszczenie osi
    ax.cla()
    
    # Ustawienie ograniczeń osi
    ax.set_xlim([-1, 1])
    ax.set_ylim([-1, 1])
    ax.set_zlim([-1, 1])
    
    # Rysowanie wektora strzałki
    ax.quiver(0, 0, 0, direction[0], direction[1], direction[2], color='r', length=1.0)

    # Oznaczenia osi
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')

# Główna pętla programu
def main():
    # Połączenie z portem szeregowym
    serial_port = serial.Serial(PORT, BAUDRATE, timeout=1)
    time.sleep(2)  # Czas na inicjalizację połączenia
    
    # Inicjalizacja wykresu 3D
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    try:
        while True:
            # Odczyt danych z seriala
            x, y, z = read_serial_data(serial_port)
            if x is not None and y is not None and z is not None:
                # Obliczanie yaw, pitch, roll
                yaw, pitch, roll = calculate_yaw_pitch_roll(x, y, z)

                # Rysowanie strzałki
                plot_arrow(ax, yaw, pitch, roll)

                # Aktualizacja wykresu
                plt.draw()
                plt.pause(DELAY)
            else:
                print("Błędne dane, spróbuję ponownie...")
    except KeyboardInterrupt:
        print("Program przerwany.")
    finally:
        serial_port.close()

if __name__ == "__main__":
    main()
