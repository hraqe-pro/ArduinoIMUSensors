import serial
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import tkinter as tk
from tkinter import ttk
from threading import Thread, Event
from queue import Queue
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from math import radians, degrees, atan2, sin, cos, sqrt, asin

# Ustawienia portu szeregowego
port = 'COM6'
baudrate = 115200

# Inicjalizacja portu szeregowego
ser = serial.Serial(port, baudrate)

# Kolejki do przechowywania danych
raw_data_queue = Queue()
calibrated_data_queue = Queue()

# Listy do przechowywania danych
raw_data = []
calibrated_data = []
segments = []  # Lista do przechowywania odcinków

# Event do zatrzymania wątku
stop_event = Event()

# Funkcja do aktualizacji wykresu
def update_plot():
    while not raw_data_queue.empty():
        raw_data.append(raw_data_queue.get())
    while not calibrated_data_queue.empty():
        calibrated_data.append(calibrated_data_queue.get())
    
    if len(raw_data) > 0 and len(calibrated_data) > 0:
        ax1.clear()
        ax2.clear()
        ax3.clear()
        ax4.clear()
        
        raw_data_np = np.array(raw_data)
        calibrated_data_np = np.array(calibrated_data)
        
        if raw_data_np.ndim == 2 and raw_data_np.shape[1] >= 3:
            ax1.scatter(raw_data_np[:, 0], raw_data_np[:, 1], raw_data_np[:, 2], c='r', marker='o')
        if calibrated_data_np.ndim == 2 and calibrated_data_np.shape[1] >= 3:
            ax2.scatter(calibrated_data_np[:, 0], calibrated_data_np[:, 1], calibrated_data_np[:, 2], c='b', marker='^')
        
        if raw_data_np.ndim == 2 and calibrated_data_np.ndim == 2 and raw_data_np.shape[1] >= 3 and calibrated_data_np.shape[1] >= 3:
            ax3.scatter(raw_data_np[:, 0], raw_data_np[:, 1], raw_data_np[:, 2], c='r', marker='o', label='Surowe')
            ax3.scatter(calibrated_data_np[:, 0], calibrated_data_np[:, 1], calibrated_data_np[:, 2], c='b', marker='^', label='Skorygowane')
        
        ax1.set_title('Dane surowe')
        ax1.set_xlabel('X')
        ax1.set_ylabel('Y')
        if isinstance(ax1, Axes3D):
            ax1.set_zlabel('Z')
        
        ax2.set_title('Dane skorygowane')
        ax2.set_xlabel('X')
        ax2.set_ylabel('Y')
        if isinstance(ax2, Axes3D):
            ax2.set_zlabel('Z')
        
        ax3.set_title('Porównanie danych')
        ax3.set_xlabel('X')
        ax3.set_ylabel('Y')
        if isinstance(ax3, Axes3D):
            ax3.set_zlabel('Z')
        ax3.legend()
        
        # Rysowanie precyzyjnego kompasa
        if calibrated_data_np.shape[0] > 0:
            x, y = calibrated_data_np[-1][0], calibrated_data_np[-1][1]
            heading = np.arctan2(y, x) * (180 / np.pi)
            ax4.arrow(0, 0, np.cos(np.deg2rad(heading)), np.sin(np.deg2rad(heading)), head_width=0.05, head_length=0.1, fc='k', ec='k')
            ax4.set_xlim(-1, 1)
            ax4.set_ylim(-1, 1)
            ax4.set_title('Kompas')
            ax4.set_aspect('equal')
            
            # Dodanie okrągłej tarczy z podziałką co pół stopnia
            circle = plt.Circle((0, 0), 1, color='black', fill=False)
            ax4.add_artist(circle)
            for angle in np.arange(0, 360, 0.5):
                x_tick = np.cos(np.deg2rad(angle))
                y_tick = np.sin(np.deg2rad(angle))
                if angle % 10 == 0:
                    ax4.text(x_tick * 1.1, y_tick * 1.1, f'{int(angle)}°', ha='center', va='center')
                else:
                    ax4.plot([x_tick * 0.95, x_tick * 1.05], [y_tick * 0.95, y_tick * 1.05], color='black')
        
        canvas.draw()

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

def plot_geographical(lat1, lon1, lat2, lon2, declination, direction, label):
    """
    Funkcja rysuje wykres na podstawie współrzędnych geograficznych oraz kąta.
    """
    # Obliczanie kąta względem północy geograficznej
    bearing = calculate_bearing(lat1, lon1, lat2, lon2)
    
    # Rysowanie linii od punktu startowego do punktu końcowego
    x1, y1 = 0, 0  # Punkt początkowy
    x2 = haversine(lat1, lon1, lat1, lon2)  # Różnica długości geograficznej w metrach
    y2 = haversine(lat1, lon1, lat2, lon1)  # Różnica szerokości geograficznej w metrach
    
    if lat2 < lat1:
        y2 = -y2
    if lon2 < lon1:
        x2 = -x2

    # Rysowanie linii od punktu startowego do punktu końcowego
    ax1.plot([x1, x2], [y1, y2], label=label)

    # Rysowanie linii reprezentującej północ geograficzną (oś Y)
    north_y = y1 + 0.2 * max(abs(x2), abs(y2))
    ax1.plot([x1, x1], [y1, north_y], 'r--', label='Północ geograficzna')

    # Obliczanie przesunięcia północy magnetycznej
    declination_rad = radians(declination)
    if direction == "zachód":
        declination_rad = -declination_rad

    mag_north_x = x1 + 0.2 * max(abs(x2), abs(y2)) * sin(declination_rad)
    mag_north_y = y1 + 0.2 * max(abs(x2), abs(y2)) * cos(declination_rad)
    ax1.plot([x1, mag_north_x], [y1, mag_north_y], 'g--', label='Północ magnetyczna')

    # Dodanie tekstów z kątami
    ax1.text((x1 + x2) / 2, (y1 + y2) / 2, f'Kąt geo: {bearing:.2f}°\n({radians(bearing):.2f} rad)', fontsize=12, color='blue')
    ax1.text(mag_north_x, mag_north_y, f'Kąt mag: {declination:.2f}°\n({radians(declination):.2f} rad)', fontsize=12, color='green')

    # Wypisanie kąta między odcinkiem a północą magnetyczną
    bearing_mag_to_segment = (declination - bearing) % 360
    ax1.text(x2, y2, f'Kąt odcinek-mag: {bearing_mag_to_segment:.2f}°\n({radians(bearing_mag_to_segment):.2f} rad)', fontsize=12, color='orange')

    # Dodatkowe ustawienia wykresu
    ax1.set_xlabel("Odległość w metrach (X)")
    ax1.set_ylabel("Odległość w metrach (Y)")
    ax1.legend()
    ax1.set_title("Kąty względem północy geograficznej i magnetycznej")
    ax1.grid(True)
    ax1.set_aspect('equal')  # Ustawienie proporcji 1:1

def add_segment():
    try:
        lat1 = float(entry_lat1.get())
        lon1 = float(entry_lon1.get())
        lat2 = float(entry_lat2.get())
        lon2 = float(entry_lon2.get())
        declination = float(entry_declination.get())
        direction = entry_direction.get().lower()
        label = entry_label.get()

        # Dodaj segment do listy
        segments.append((lat1, lon1, lat2, lon2, declination, direction, label))

        # Wyczyszczenie wykresu i ponowne rysowanie wszystkich segmentów
        ax1.clear()
        for segment in segments:
            lat1, lon1, lat2, lon2, declination, direction, label = segment
            plot_geographical(lat1, lon1, lat2, lon2, declination, direction, label)
        canvas2.draw()
    except ValueError:
        print("Błędne dane wejściowe. Upewnij się, że wszystkie pola są wypełnione poprawnie.")

def remove_segment():
    try:
        selected_label = entry_remove_label.get()

        # Filtruj segmenty, aby usunąć ten z wybraną nazwą
        global segments
        segments = [seg for seg in segments if seg[6] != selected_label]

        # Wyczyszczenie wykresu i ponowne rysowanie wszystkich segmentów
        ax1.clear()
        for segment in segments:
            lat1, lon1, lat2, lon2, declination, direction, label = segment
            plot_geographical(lat1, lon1, lat2, lon2, declination, direction, label)
        canvas2.draw()
    except ValueError:
        print("Błędne dane wejściowe. Upewnij się, że wszystkie pola są wypełnione poprawnie.")

def read_serial():
    try:
        while not stop_event.is_set():
            line = ser.readline().decode('utf-8').strip()
            if "RAW" in line and "CORRECTED" in line:
                parts = line.split(" | ")
                raw_str = parts[0].replace("RAW: ", "")
                calibrated_str = parts[1].replace("CORRECTED: ", "")
                
                raw = list(map(float, raw_str.split(',')))
                calibrated = list(map(float, calibrated_str.split(',')))
                
                raw_data_queue.put(raw)
                calibrated_data_queue.put(calibrated)
    finally:
        ser.close()

def plot_updater():
    if not stop_event.is_set():
        update_plot()
        root.after(100, plot_updater)

def close_program():
    stop_event.set()
    root.quit()
    root.destroy()

def enlarge_compass():
    ax1.set_visible(False)
    ax2.set_visible(False)
    ax3.set_visible(False)
    ax4.set_position([0.1, 0.1, 0.8, 0.8])
    enlarge_button.pack_forget()
    shrink_button.pack()

def shrink_compass():
    ax1.set_visible(True)
    ax2.set_visible(True)
    ax3.set_visible(True)
    ax4.set_position([0.55, 0.1, 0.35, 0.35])
    shrink_button.pack_forget()
    enlarge_button.pack()

def calculate_plot():
    try:
        lat1 = float(entry_lat1.get())
        lon1 = float(entry_lon1.get())
        lat2 = float(entry_lat2.get())
        lon2 = float(entry_lon2.get())
        declination = float(entry_declination.get())
        direction = entry_direction.get().lower()
        label = entry_label.get()

        # Wyczyszczenie poprzednich rysunków
        ax1.clear()

        # Rysowanie wykresu geograficznego
        plot_geographical(lat1, lon1, lat2, lon2, declination, direction, label)
        canvas2.draw()

    except ValueError:
        print("Błędne dane wejściowe. Upewnij się, że wszystkie pola są wypełnione poprawnie.")

# Inicjalizacja GUI
root = tk.Tk()
root.title("Magnetometr - Wizualizacja danych")
root.geometry("1200x800")  # Ustawienie rozmiaru okna

# Zakładki
tab_control = ttk.Notebook(root)

# Zakładka 1 - Tryb kalibracji
tab1 = ttk.Frame(tab_control)
tab_control.add(tab1, text="Tryb Kalibracji i Wizualizacji")

# Zakładka 2 - Specjalny tryb
tab2 = ttk.Frame(tab_control)
tab_control.add(tab2, text="Specjalny Tryb Geograficzny")

# Umieszczanie zakładek w głównym oknie
tab_control.pack(expand=1, fill="both")

# Wykresy dla zakładki 1
fig = plt.figure()
ax1 = fig.add_subplot(221, projection='3d')
ax2 = fig.add_subplot(222, projection='3d')
ax3 = fig.add_subplot(223, projection='3d')
ax4 = fig.add_subplot(224)

canvas = FigureCanvasTkAgg(fig, master=tab1)
canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)

# Przyciski do powiększania i zmniejszania kompasa
enlarge_button = tk.Button(tab1, text="Powiększ kompas", command=enlarge_compass)
enlarge_button.pack(side=tk.LEFT)

shrink_button = tk.Button(tab1, text="Zmniejsz kompas", command=shrink_compass)
shrink_button.pack(side=tk.LEFT)
shrink_button.pack_forget()

# Przycisk zamknięcia programu w zakładce 1
tk.Button(tab1, text="Zakończ", command=close_program).pack()

# Interfejs użytkownika dla zakładki specjalnej
fig2 = plt.figure()
ax1 = fig2.add_subplot(111)
canvas2 = FigureCanvasTkAgg(fig2, master=tab2)
canvas2.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)

tk.Label(tab2, text="Szerokość geograficzna punktu 1").pack()
entry_lat1 = tk.Entry(tab2)
entry_lat1.pack()

tk.Label(tab2, text="Długość geograficzna punktu 1").pack()
entry_lon1 = tk.Entry(tab2)
entry_lon1.pack()

tk.Label(tab2, text="Szerokość geograficzna punktu 2").pack()
entry_lat2 = tk.Entry(tab2)
entry_lat2.pack()

tk.Label(tab2, text="Długość geograficzna punktu 2").pack()
entry_lon2 = tk.Entry(tab2)
entry_lon2.pack()

tk.Label(tab2, text="Deklinacja magnetyczna (w stopniach)").pack()
entry_declination = tk.Entry(tab2)
entry_declination.pack()

tk.Label(tab2, text="Kierunek deklinacji (wschód/zachód)").pack()
entry_direction = tk.Entry(tab2)
entry_direction.pack()

tk.Label(tab2, text="Nazwa odcinka").pack()
entry_label = tk.Entry(tab2)
entry_label.pack()

tk.Button(tab2, text="Dodaj odcinek", command=add_segment).pack()

tk.Label(tab2, text="Nazwa odcinka do usunięcia").pack()
entry_remove_label = tk.Entry(tab2)
entry_remove_label.pack()

tk.Button(tab2, text="Usuń odcinek", command=remove_segment).pack()

# Inicjalizacja pustego wykresu w zakładce specjalnej
ax1.set_title("Pusty wykres geograficzny")
ax1.set_xlabel("Odległość w metrach (X)")
ax1.set_ylabel("Odległość w metrach (Y)")
ax1.grid(True)
canvas2.draw()

# Uruchomienie wątku do odczytu danych z portu szeregowego
thread = Thread(target=read_serial)
thread.start()

# Uruchomienie aktualizacji wykresu
root.after(100, plot_updater)

# Uruchomienie pętli głównej Tkinter
root.protocol("WM_DELETE_WINDOW", close_program)
root.mainloop()
