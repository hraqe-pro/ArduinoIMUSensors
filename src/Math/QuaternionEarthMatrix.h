#ifndef QUATERNIONEARTHMATRIX_H
#define QUATERNIONEARTHMATRIX_H
struct QuaternionEarthMatrix
{
  double w = 1, x = 0, y = 0, z = 0;

  void normalize();
  void fromAzimuth(double azimuthRadians);
  double dotProduct(const QuaternionEarthMatrix &other) const;

  QuaternionEarthMatrix slerp(const QuaternionEarthMatrix& other, double t);

  QuaternionEarthMatrix operator*(double scalar) const {
        return {w * scalar, x * scalar, y * scalar, z * scalar};
  }

  QuaternionEarthMatrix operator+(const QuaternionEarthMatrix& other) const {
        return {w + other.w, x + other.x, y + other.y, z + other.z};
  }
};

#endif