import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import math

def yaw_to_quaternion(yaw):
    qw = np.cos(yaw / 2)
    qx = 0
    qy = 0
    qz = np.sin(yaw / 2)
    return np.array([qw, qx, qy, qz])

def quaternion_multiply(q1, q2):
    w1, x1, y1, z1 = q1
    w2, x2, y2, z2 = q2
    w = w1*w2 - x1*x2 - y1*y2 - z1*z2
    x = w1*x2 + x1*w2 + y1*z2 - z1*y2
    y = w1*y2 - x1*z2 + y1*w2 + z1*x2
    z = w1*z2 + x1*y2 - y1*x2 + z1*w2
    return np.array([w, x, y, z])

def rotate_vector_by_yaw(vector, yaw):
    quaternion = yaw_to_quaternion(yaw)
    q_vector = np.concatenate(([0], vector))
    q_conjugate = np.array([quaternion[0], -quaternion[1], -quaternion[2], -quaternion[3]])
    rotated_vector = quaternion_multiply(quaternion_multiply(quaternion, q_vector), q_conjugate)
    return rotated_vector[1:]

# Żółty wektor początkowy wzdłuż osi Y
initial_vector = np.array([0, 1, 0])  

# Prędkość kątowa i czas
angular_velocity = 1.0471975511939  # rad/s
time = 1  # s
yaw = angular_velocity * time  # Obrót o 10 radianów w 1 sekundę

# Obrót żółtego wektora
rotated_vector = rotate_vector_by_yaw(initial_vector, yaw)

# Rysowanie wektorów
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.quiver(0, 0, 0, initial_vector[0], initial_vector[1], initial_vector[2], color='y', label='Początkowy wektor')
ax.quiver(0, 0, 0, rotated_vector[0], rotated_vector[1], rotated_vector[2], color='b', label='Obrócony wektor')
ax.set_xlim([-1, 1])
ax.set_ylim([-1, 1])
ax.set_zlim([-1, 1])
ax.set_xlabel('X axis')
ax.set_ylabel('Y axis')
ax.set_zlabel('Z axis')
plt.legend()
plt.show()
