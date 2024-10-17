#include "QuaternionEarthMatrix.h"
#include <cmath> 
#include <algorithm>

void QuaternionEarthMatrix::normalize()
{
    double norm = std::sqrt(w * w + x * x + y * y + z * z);
    w /= norm;
    x /= norm;
    y /= norm;
    z /= norm;   
}

void QuaternionEarthMatrix::fromAzimuth(double azimuthRadians)
{
    double halfAngle = azimuthRadians / 2.0;
    w = cosf(halfAngle);
    x = 0.0;
    y = 0.0;
    z = sinf(halfAngle);
}

double QuaternionEarthMatrix::dotProduct(const QuaternionEarthMatrix& other) const {
    return w * other.w + x * other.x + y * other.y + z * other.z;
}

QuaternionEarthMatrix QuaternionEarthMatrix::slerp(const QuaternionEarthMatrix &other, double t)
{
    double dotProduct = this->dotProduct(other);

    // Upewniamy się, że dotProduct jest w zakresie [-1, 1] z uwzględnieniem błędów numerycznych
    dotProduct = std::max(-1.0, std::min(1.0, dotProduct));

    // Jeżeli iloczyn skalarny jest bardzo bliski 1, używamy przybliżenia
    if (dotProduct > 0.999999) {
        // Używamy linearnej interpolacji, aby uniknąć problemów numerycznych
        QuaternionEarthMatrix result = *this * (1.0f - t) + other * t;
        result.normalize();
        return result;
    }

    // Normalne SLERP
    double theta_0 = std::acos(dotProduct);   // Kąt między kwaternionami
    double sin_theta_0 = std::sin(theta_0);   // Sinus kąta

    double theta = theta_0 * t;               // Kąt skalowany przez t
    double sin_theta = std::sin(theta);       // Sinus nowego kąta

    double s1 = std::cos(theta) - dotProduct * sin_theta / sin_theta_0;
    double s2 = sin_theta / sin_theta_0;

    return *this * s1 + other * s2;
}
