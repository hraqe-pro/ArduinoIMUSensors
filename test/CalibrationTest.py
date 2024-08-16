import serial
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import tkinter as tk
from threading import Thread
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

# Ustawienia portu szeregowego
port = 'COM6'
baudrate = 115200

# Inicjalizacja portu szeregowego
ser = serial.Serial(port, baudrate)

# Listy do przechowywania danych
raw_data = []
calibrated_data = []

# Funkcja do aktualizacji wykresu
def update_plot():
    ax1.clear()
    ax2.clear()
    ax3.clear()
    
    ax1.scatter(np.array(raw_data)[:, 0], np.array(raw_data)[:, 1], np.array(raw_data)[:, 2], c='r', marker='o')
    ax1.set_title('Dane surowe')
    ax1.set_xlabel('X')
    ax1.set_ylabel('Y')
    ax1.set_zlabel('Z')
    
    ax2.scatter(np.array(calibrated_data)[:, 0], np.array(calibrated_data)[:, 1], np.array(calibrated_data)[:, 2], c='b', marker='^')
    ax2.set_title('Dane skorygowane')
    ax2.set_xlabel('X')
    ax2.set_ylabel('Y')
    ax2.set_zlabel('Z')
    
    ax3.scatter(np.array(raw_data)[:, 0], np.array(raw_data)[:, 1], np.array(raw_data)[:, 2], c='r', marker='o', label='Surowe')
    ax3.scatter(np.array(calibrated_data)[:, 0], np.array(calibrated_data)[:, 1], np.array(calibrated_data)[:, 2], c='b', marker='^', label='Skorygowane')
    ax3.set_title('Porównanie danych')
    ax3.set_xlabel('X')
    ax3.set_ylabel('Y')
    ax3.set_zlabel('Z')
    ax3.legend()
    
    canvas.draw()

# Funkcja do odczytu danych z portu szeregowego
def read_serial():
    try:
        while True:
            line = ser.readline().decode('utf-8').strip()
            if "RAW" in line and "CORRECTED" in line:
                parts = line.split(" | ")
                raw_str = parts[0].replace("RAW: ", "")
                calibrated_str = parts[1].replace("CORRECTED: ", "")
                
                raw = list(map(float, raw_str.split(',')))
                calibrated = list(map(float, calibrated_str.split(',')))
                
                raw_data.append(raw)
                calibrated_data.append(calibrated)
                
                root.after(0, update_plot)  # Aktualizacja GUI w głównym wątku
    finally:
        ser.close()

# Funkcja do zamknięcia programu
def close_program():
    root.quit()
    root.destroy()

# Inicjalizacja GUI
root = tk.Tk()
root.title("Magnetometr - Wizualizacja danych")
root.geometry("1200x800")  # Ustawienie rozmiaru okna

# Dodanie przycisku "Zakończ"
button = tk.Button(root, text="Zakończ", command=close_program)
button.pack()

# Dodanie płótna do rysowania wykresów
canvas_frame = tk.Frame(root)
canvas_frame.pack(fill=tk.BOTH, expand=True)

# Dodanie wykresów do płótna
fig = plt.figure(figsize=(18, 6))
ax1 = fig.add_subplot(131, projection='3d')
ax2 = fig.add_subplot(132, projection='3d')
ax3 = fig.add_subplot(133, projection='3d')

canvas = FigureCanvasTkAgg(fig, master=canvas_frame)
canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

# Uruchomienie wątku do odczytu danych z portu szeregowego
thread = Thread(target=read_serial)
thread.start()

# Uruchomienie GUI
root.mainloop()




