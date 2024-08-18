import serial
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import tkinter as tk
from threading import Thread, Event
from queue import Queue
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

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
        ax1.set_zlabel('Z')
        
        ax2.set_title('Dane skorygowane')
        ax2.set_xlabel('X')
        ax2.set_ylabel('Y')
        ax2.set_zlabel('Z')
        
        ax3.set_title('Porównanie danych')
        ax3.set_xlabel('X')
        ax3.set_ylabel('Y')
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

# Funkcja do odczytu danych z portu szeregowego
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

# Funkcja do aktualizacji wykresu w głównym wątku
def plot_updater():
    if not stop_event.is_set():
        update_plot()
        root.after(100, plot_updater)

# Funkcja do zamknięcia programu
def close_program():
    stop_event.set()
    root.quit()
    root.destroy()

# Funkcja do powiększenia kompasa
def enlarge_compass():
    ax1.set_visible(False)
    ax2.set_visible(False)
    ax3.set_visible(False)
    ax4.set_position([0.1, 0.1, 0.8, 0.8])
    enlarge_button.pack_forget()
    shrink_button.pack()

# Funkcja do zmniejszenia kompasa
def shrink_compass():
    ax1.set_visible(True)
    ax2.set_visible(True)
    ax3.set_visible(True)
    ax4.set_position([0.55, 0.1, 0.35, 0.35])
    shrink_button.pack_forget()
    enlarge_button.pack()

# Inicjalizacja GUI
root = tk.Tk()
root.title("Magnetometr - Wizualizacja danych")
root.geometry("1200x800")  # Ustawienie rozmiaru okna

# Dodanie przycisku "Zakończ"
button = tk.Button(root, text="Zakończ", command=close_program)
button.pack()

# Dodanie przycisku do powiększenia kompasa
enlarge_button = tk.Button(root, text="Powiększ kompas", command=enlarge_compass)
enlarge_button.pack()

# Dodanie przycisku do zmniejszenia kompasa
shrink_button = tk.Button(root, text="Zmniejsz kompas", command=shrink_compass)
shrink_button.pack_forget()

# Dodanie płótna do rysowania wykresów
canvas_frame = tk.Frame(root)
canvas_frame.pack(fill=tk.BOTH, expand=True)

# Dodanie wykresów do płótna
fig = plt.figure(figsize=(18, 12))  # Zwiększenie rozmiaru figury
ax1 = fig.add_subplot(231, projection='3d')
ax2 = fig.add_subplot(232, projection='3d')
ax3 = fig.add_subplot(233, projection='3d')
ax4 = fig.add_subplot(212)  # Dodanie osi dla kompasa

canvas = FigureCanvasTkAgg(fig, master=canvas_frame)
canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

# Uruchomienie wątku do odczytu danych z portu szeregowego
thread = Thread(target=read_serial)
thread.start()

# Uruchomienie wątku do aktualizacji wykresu
root.after(100, plot_updater)

# Uruchomienie GUI
root.mainloop()




