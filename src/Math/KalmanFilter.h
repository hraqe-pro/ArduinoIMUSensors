#include <array>
#include <cmath>

class KalmanFilter3D {
public:
    KalmanFilter3D(double dt, double processNoise, double measurementNoise)
        : dt(dt) {
        // Initialize state transition matrix A
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                A[i][j] = (i == j) ? 1.0 : 0.0;
            }
            if (i < 3) {
                A[i][i + 3] = dt; // State transition for angular velocities
            }
        }

        // Initialize process noise covariance matrix Q
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                Q[i][j] = (i == j) ? processNoise : 0.0;
            }
        }

        // Initialize error covariance matrix P
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                P[i][j] = (i == j) ? 1.0 : 0.0;
            }
        }

        // Initialize measurement matrix H
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 6; ++j) {
                H[i][j] = (i == j) ? 1.0 : 0.0;
            }
        }

        // Initialize measurement noise covariance matrix R
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                R[i][j] = (i == j) ? measurementNoise : 0.0;
            }
        }
    }

    void predict() {
        // Predict state X = A * X
        double X_pred[6] = { 0.0 };
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                X_pred[i] += A[i][j] * X[j];
            }
        }
        for (int i = 0; i < 6; ++i) {
            X[i] = X_pred[i];
        }

        // Predict covariance P = A * P * A^T + Q
        double P_pred[6][6] = { 0.0 };
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                for (int k = 0; k < 6; ++k) {
                    P_pred[i][j] += A[i][k] * P[k][j];
                }
            }
        }
        double P_updated[6][6] = { 0.0 };
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                P_updated[i][j] = P_pred[i][j] + Q[i][j];
            }
        }
        // Update the error covariance
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                P[i][j] = P_updated[i][j];
            }
        }
    }

    void update(const std::array<double, 3>& omega) {
        // Compute innovation
        double Y[3] = { 0.0 };
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 6; ++j) {
                Y[i] += H[i][j] * X[j];
            }
            Y[i] = omega[i] - Y[i]; // z - H * X
        }

        // Compute Kalman gain K = P * H^T * inv(S)
        double S[3][3] = { 0.0 }; // Innovation covariance
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                for (int k = 0; k < 6; ++k) {
                    S[i][j] += H[i][k] * P[k][j];
                }
                S[i][j] += (i == j) ? R[i][j] : 0.0; // Add measurement noise
            }
        }

        // Inverse of S
        double invS[3][3];
        double det = S[0][0] * (S[1][1] * S[2][2] - S[2][1] * S[1][2]) -
            S[0][1] * (S[1][0] * S[2][2] - S[2][0] * S[1][2]) +
            S[0][2] * (S[1][0] * S[2][1] - S[2][0] * S[1][1]);
        invS[0][0] = (S[1][1] * S[2][2] - S[2][1] * S[1][2]) / det;
        invS[0][1] = (S[0][2] * S[2][1] - S[0][1] * S[2][2]) / det;
        invS[0][2] = (S[0][1] * S[1][2] - S[0][2] * S[1][1]) / det;
        invS[1][0] = (S[1][2] * S[2][0] - S[1][0] * S[2][2]) / det;
        invS[1][1] = (S[0][0] * S[2][2] - S[0][2] * S[2][0]) / det;
        invS[1][2] = (S[0][2] * S[1][0] - S[0][0] * S[1][2]) / det;
        invS[2][0] = (S[1][0] * S[2][1] - S[1][1] * S[2][0]) / det;
        invS[2][1] = (S[0][1] * S[2][0] - S[0][0] * S[2][1]) / det;
        invS[2][2] = (S[0][0] * S[1][1] - S[0][1] * S[1][0]) / det;

        // Compute Kalman gain K
        double K[6][3] = { 0.0 };
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 3; ++j) {
                for (int k = 0; k < 6; ++k) {
                    K[i][j] += P[i][k] * H[j][k];
                }
                // Multiply by inverse of S
                double sum = 0.0;
                for (int l = 0; l < 3; ++l) {
                    sum += K[i][l] * invS[l][j];
                }
                K[i][j] = sum;
            }
        }

        // Update state X = X + K * Y
        for (int i = 0; i < 6; ++i) {
            double correction = 0.0;
            for (int j = 0; j < 3; ++j) {
                correction += K[i][j] * Y[j];
            }
            X[i] += correction;
        }

        // Update the error covariance P = (I - K * H) * P
        double KH[6][6] = { 0.0 };
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                for (int k = 0; k < 3; ++k) {
                    KH[i][j] += K[i][k] * H[k][j];
                }
            }
        }

        // Compute (I - K * H) * P
        double I_KH[6][6] = { 0.0 };
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                I_KH[i][j] = (i == j ? 1.0 : 0.0) - KH[i][j];
            }
        }

        double P_updated_final[6][6] = { 0.0 };
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                for (int k = 0; k < 6; ++k) {
                    P_updated_final[i][j] += I_KH[i][k] * P[k][j];
                }
            }
        }

        // Update error covariance
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                P[i][j] = P_updated_final[i][j];
            }
        }
    }

    // Accessor for the estimated angles
    std::array<double, 3> getAngles() const {
        return { X[0], X[1], X[2] };
    }

private:
    double dt; // Time step
    double X[6] = { 0.0 }; // State vector: [angle_x, angle_y, angle_z, angular_velocity_x, angular_velocity_y, angular_velocity_z]
    double A[6][6]; // State transition matrix
    double Q[6][6]; // Process noise covariance
    double P[6][6]; // Error covariance
    double H[3][6]; // Measurement matrix
    double R[3][3]; // Measurement noise covariance
};