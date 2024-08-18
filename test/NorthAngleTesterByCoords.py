import numpy as np
import matplotlib.pyplot as plt
from math import radians, degrees, atan2, sin, cos, sqrt, asin

def haversine(lat1, lon1, lat2, lon2):
    """
    Funkcja oblicza wielki krąg między dwoma punktami na sferze (odległość w metrach).
    """
    R = 6371000  # Promień Ziemi w metrach
    dlat = radians(lat2 - lat1)
    dlon = radians(lon2 - lon1)
    lat1 = radians(lat1)
    lat2 = radians(lat2)

    a = sin(dlat / 2)**2 + cos(lat1) * cos(lat2) * sin(dlon / 2)**2
    c = 2 * asin(sqrt(a))

    distance = R * c
    return distance

def calculate_bearing(lat1, lon1, lat2, lon2):
    """
    Funkcja oblicza kąt (bearing) między odcinkiem łączącym dwa punkty a północą geograficzną.
    Kąt liczony jest przy punkcie bardziej wysuniętym na północ.
    """
    dLon = radians(lon2 - lon1)
    lat1 = radians(lat1)
    lat2 = radians(lat2)

    x = sin(dLon) * cos(lat2)
    y = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon)

    initial_bearing = atan2(x, y)
    
    # Przekonwertuj wynik na stopnie i ustaw zakres 0-360
    initial_bearing = degrees(initial_bearing)
    bearing = (initial_bearing + 360) % 360
    
    return bearing

def plot_triangle(lat1, lon1, lat2, lon2, bearing, mag_bearing, bearing_mag_to_segment):
    """
    Funkcja rysuje trójkąt na podstawie współrzędnych geograficznych oraz kąta, uwzględniając rzeczywiste odległości na Ziemi.
    """
    # Obliczanie odległości między punktami w metrach
    distance = haversine(lat1, lon1, lat2, lon2)
    
    # Konwersja współrzędnych geograficznych na płaskie współrzędne
    x1, y1 = 0, 0  # Pierwszy punkt na początku
    x2 = haversine(lat1, lon1, lat1, lon2)  # Różnica długości geograficznej w metrach
    y2 = haversine(lat1, lon1, lat2, lon1)  # Różnica szerokości geograficznej w metrach
    
    if lat2 < lat1:
        y2 = -y2
    if lon2 < lon1:
        x2 = -x2

    # Rysowanie linii od punktu startowego do punktu końcowego
    plt.plot([x1, x2], [y1, y2], 'b', label='Odcinek')

    # Rysowanie linii reprezentującej północ geograficzną (linia pionowa z punktu startowego)
    north_x = x1
    north_y = y1 + 0.2 * distance  # 20% długości odcinka
    plt.plot([x1, north_x], [y1, north_y], 'r--', label='Północ geograficzna')

    # Rysowanie linii reprezentującej północ magnetyczną
    angle_rad = radians(mag_bearing)
    magnet_north_x = x1 + 0.2 * sin(angle_rad) * distance
    magnet_north_y = y1 + 0.2 * cos(angle_rad) * distance
    plt.plot([x1, magnet_north_x], [y1, magnet_north_y], 'g--', label='Północ magnetyczna')

    # Zaznaczenie interesujących nas kątów
    plt.text((x1 + x2) / 2, (y1 + y2) / 2, f'Kąt geo: {bearing:.2f}°\n({radians(bearing):.2f} rad)', fontsize=12, color='blue')
    plt.text(magnet_north_x, magnet_north_y, f'Kąt mag: {mag_bearing:.2f}°\n({radians(mag_bearing):.2f} rad)', fontsize=12, color='green')

    # Wypisanie kąta między odcinkiem a północą magnetyczną
    plt.text(x2, y2, f'Kąt odcinek-mag: {bearing_mag_to_segment:.2f}°\n({radians(bearing_mag_to_segment):.2f} rad)', fontsize=12, color='orange')

    # Dodatkowe ustawienia wykresu
    plt.xlabel("Odległość w metrach (X)")
    plt.ylabel("Odległość w metrach (Y)")
    plt.legend()
    plt.title("Kąty względem północy geograficznej i magnetycznej")
    plt.grid(True)
    plt.axis('equal')  # Ustawienie proporcji 1:1
    plt.show()

def main():
    # Pobieranie współrzędnych od użytkownika
    lat1 = float(input("Podaj szerokość geograficzną pierwszego punktu: "))
    lon1 = float(input("Podaj długość geograficzną pierwszego punktu: "))
    lat2 = float(input("Podaj szerokość geograficzną drugiego punktu: "))
    lon2 = float(input("Podaj długość geograficzną drugiego punktu: "))
    
    # Pobieranie poprawki magnetycznej od użytkownika
    declination = float(input("Podaj wartość poprawki magnetycznej (w stopniach): "))
    direction = input("Podaj kierunek poprawki magnetycznej (wpisz 'wschód' lub 'zachód'): ").strip().lower()

    # Sprawdzenie kierunku poprawki magnetycznej
    if direction == 'wschód':
        mag_bearing = (declination) % 360
    elif direction == 'zachód':
        mag_bearing = (-declination) % 360
    else:
        print("Nieprawidłowy kierunek poprawki. Użyj 'wschód' lub 'zachód'.")
        return

    # Obliczanie kąta względem północy geograficznej
    bearing = calculate_bearing(lat1, lon1, lat2, lon2)
    
    # Obliczenie kąta między odcinkiem a północą magnetyczną
    bearing_mag_to_segment = (bearing - mag_bearing) % 360

    # Rysowanie wykresu
    plot_triangle(lat1, lon1, lat2, lon2, bearing, mag_bearing, bearing_mag_to_segment)

if __name__ == "__main__":
    main()
