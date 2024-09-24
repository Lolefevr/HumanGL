// my_math.h
#ifndef MY_MATH_H
#define MY_MATH_H

#include <cmath>
#include <vector>

struct Vec3 {
  float x, y, z;

  Vec3();
  Vec3(float x, float y, float z);

  Vec3 operator+(const Vec3& other) const;
  Vec3 operator-(const Vec3& other) const;
  Vec3 operator*(float scalar) const;
  static Vec3 cross(const Vec3& a, const Vec3& b);
  static float dot(const Vec3& a, const Vec3& b);
  static Vec3 normalize(const Vec3& v);
};

struct Mat4 {
  float m[16];

  Mat4();
  static Mat4 identity();
  static Mat4 translate(const Mat4& mat, const Vec3& vec);
  static Mat4 rotate(const Mat4& mat, float angle, const Vec3& axis);
  static Mat4 scale(const Mat4& mat, const Vec3& vec);
  static Mat4 perspective(float fov, float aspect, float near, float far);
  static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up);
  static Mat4 multiply(const Mat4& a, const Mat4& b);
};

class MatrixStack {
 public:
  MatrixStack();
  void push(const Mat4& mat);
  Mat4 pop();
  Mat4 top() const;

 private:
  std::vector<Mat4> stack;
};

#endif  // MY_MATH_H
