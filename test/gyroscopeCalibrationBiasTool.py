import os
import csv
import tkinter as tk
from tkinter import filedialog

# Funkcja do wyboru pliku CSV
def wybierz_plik():
    root = tk.Tk()
    root.withdraw()  # Ukrywa okno główne
    plik_csv = filedialog.askopenfilename(initialdir=os.getcwd(), title="Wybierz plik CSV", filetypes=(("CSV files", "*.csv"), ("all files", "*.*")))
    return plik_csv

# Funkcja obliczająca średnie z wybranych kolumn
def oblicz_srednie(plik_csv):
    kolumna_4 = []
    kolumna_5 = []
    kolumna_6 = []

    # Otwórz plik CSV i odczytaj dane
    with open(plik_csv, newline='', encoding='utf-8') as csvfile:
        reader = csv.reader(csvfile, delimiter=';')
        for row in reader:
            if len(row) >= 6:  # Upewnij się, że są co najmniej 6 kolumn
                # Dodaj wartości do odpowiednich kolumn, po konwersji na float
                try:
                    kolumna_4.append(float(row[3]))
                    kolumna_5.append(float(row[4]))
                    kolumna_6.append(float(row[5]))
                except ValueError:
                    # Pomijamy wiersze, w których są nieprawidłowe dane (np. tekst)
                    pass

    # Oblicz średnie dla każdej kolumny
    srednia_4 = sum(kolumna_4) / len(kolumna_4) if kolumna_4 else 0
    srednia_5 = sum(kolumna_5) / len(kolumna_5) if kolumna_5 else 0
    srednia_6 = sum(kolumna_6) / len(kolumna_6) if kolumna_6 else 0

    # Podziel przez 100000000
    wynik_4 = srednia_4 / 100000000
    wynik_5 = srednia_5 / 100000000
    wynik_6 = srednia_6 / 100000000

    return wynik_4, wynik_5, wynik_6

# Funkcja główna programu
def main():
    plik_csv = wybierz_plik()
    if not plik_csv:
        print("Nie wybrano pliku.")
        return

    wynik_4, wynik_5, wynik_6 = oblicz_srednie(plik_csv)

    # Wyświetl wyniki
    print(f"Wynik dla kolumny x: {wynik_4}")
    print(f"Wynik dla kolumny y: {wynik_5}")
    print(f"Wynik dla kolumny z: {wynik_6}")

if __name__ == "__main__":
    main()
