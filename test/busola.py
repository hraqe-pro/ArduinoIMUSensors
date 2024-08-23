import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D

def draw_compass_with_direction(direction):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    # Rysowanie kuli
    u = np.linspace(0, 2 * np.pi, 100)
    v = np.linspace(0, np.pi, 100)
    x = 0.5 * np.outer(np.cos(u), np.sin(v))
    y = 0.5 * np.outer(np.sin(u), np.sin(v))
    z = 0.5 * np.outer(np.ones(np.size(u)), np.cos(v))
    ax.plot_surface(x, y, z, color='b', alpha=0.3)

    # Rysowanie linii wskazującej kierunek
    ax.quiver(0, 0, 0, direction[0], direction[1], direction[2], color='r', length=0.5)

    # Ustawienia osi
    ax.set_xlim([-1, 1])
    ax.set_ylim([-1, 1])
    ax.set_zlim([-1, 1])

    # Etykiety osi
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')

    plt.show()

# Przykładowy kierunek (wektor)
direction = [1, 1, 1]
draw_compass_with_direction(direction)