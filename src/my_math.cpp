// my_math.cpp
#include "my_math.h"

// Implémentations de Vec3
// (Identiques à la version précédente)

Vec3::Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

Vec3 Vec3::operator+(const Vec3& other) const {
  return Vec3(x + other.x, y + other.y, z + other.z);
}

Vec3 Vec3::operator-(const Vec3& other) const {
  return Vec3(x - other.x, y - other.y, z - other.z);
}

Vec3 Vec3::operator*(float scalar) const {
  return Vec3(x * scalar, y * scalar, z * scalar);
}

Vec3 Vec3::cross(const Vec3& a, const Vec3& b) {
  return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
              a.x * b.y - a.y * b.x);
}

float Vec3::dot(const Vec3& a, const Vec3& b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 Vec3::normalize(const Vec3& v) {
  float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  if (length == 0.0f) return Vec3(0.0f, 0.0f, 0.0f);
  return Vec3(v.x / length, v.y / length, v.z / length);
}

// Implémentations de Mat4

Mat4::Mat4() {
  for (int i = 0; i < 16; ++i) m[i] = 0.0f;
}

Mat4 Mat4::identity() {
  Mat4 result;
  result.m[0 + 0 * 4] = 1.0f;
  result.m[1 + 1 * 4] = 1.0f;
  result.m[2 + 2 * 4] = 1.0f;
  result.m[3 + 3 * 4] = 1.0f;
  return result;
}

Mat4 Mat4::translate(const Mat4& mat, const Vec3& vec) {
  Mat4 result = mat;
  result.m[12] =
      mat.m[0] * vec.x + mat.m[4] * vec.y + mat.m[8] * vec.z + mat.m[12];
  result.m[13] =
      mat.m[1] * vec.x + mat.m[5] * vec.y + mat.m[9] * vec.z + mat.m[13];
  result.m[14] =
      mat.m[2] * vec.x + mat.m[6] * vec.y + mat.m[10] * vec.z + mat.m[14];
  result.m[15] =
      mat.m[3] * vec.x + mat.m[7] * vec.y + mat.m[11] * vec.z + mat.m[15];
  return result;
}

Mat4 Mat4::scale(const Mat4& mat, const Vec3& vec) {
  Mat4 result = mat;
  result.m[0] *= vec.x;
  result.m[1] *= vec.x;
  result.m[2] *= vec.x;
  result.m[3] *= vec.x;

  result.m[4] *= vec.y;
  result.m[5] *= vec.y;
  result.m[6] *= vec.y;
  result.m[7] *= vec.y;

  result.m[8] *= vec.z;
  result.m[9] *= vec.z;
  result.m[10] *= vec.z;
  result.m[11] *= vec.z;

  return result;
}

Mat4 Mat4::rotate(const Mat4& mat, float angle, const Vec3& axis) {
  Mat4 result;
  Vec3 normAxis = Vec3::normalize(axis);
  float c = std::cos(angle);
  float s = std::sin(angle);
  float oneMinusC = 1.0f - c;

  float x = normAxis.x;
  float y = normAxis.y;
  float z = normAxis.z;

  Mat4 rot;
  rot.m[0] = x * x * oneMinusC + c;
  rot.m[1] = x * y * oneMinusC + z * s;
  rot.m[2] = x * z * oneMinusC - y * s;
  rot.m[3] = 0.0f;

  rot.m[4] = y * x * oneMinusC - z * s;
  rot.m[5] = y * y * oneMinusC + c;
  rot.m[6] = y * z * oneMinusC + x * s;
  rot.m[7] = 0.0f;

  rot.m[8] = z * x * oneMinusC + y * s;
  rot.m[9] = z * y * oneMinusC - x * s;
  rot.m[10] = z * z * oneMinusC + c;
  rot.m[11] = 0.0f;

  rot.m[12] = 0.0f;
  rot.m[13] = 0.0f;
  rot.m[14] = 0.0f;
  rot.m[15] = 1.0f;

  result = multiply(mat, rot);
  return result;
}

Mat4 Mat4::multiply(const Mat4& a, const Mat4& b) {
  Mat4 result;
  for (int col = 0; col < 4; ++col) {
    for (int row = 0; row < 4; ++row) {
      result.m[row + col * 4] = a.m[row + 0 * 4] * b.m[0 + col * 4] +
                                a.m[row + 1 * 4] * b.m[1 + col * 4] +
                                a.m[row + 2 * 4] * b.m[2 + col * 4] +
                                a.m[row + 3 * 4] * b.m[3 + col * 4];
    }
  }
  return result;
}

Mat4 Mat4::perspective(float fov, float aspect, float near, float far) {
  Mat4 result;
  float tanHalfFovy = std::tan(fov / 2.0f);

  result.m[0] = 1.0f / (aspect * tanHalfFovy);
  result.m[5] = 1.0f / tanHalfFovy;
  result.m[10] = -(far + near) / (far - near);
  result.m[11] = -1.0f;
  result.m[14] = -(2.0f * far * near) / (far - near);
  return result;
}

Mat4 Mat4::lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
  Vec3 f = Vec3::normalize(center - eye);
  Vec3 s = Vec3::normalize(Vec3::cross(f, up));
  Vec3 u = Vec3::cross(s, f);

  Mat4 result = Mat4::identity();
  result.m[0] = s.x;
  result.m[4] = s.y;
  result.m[8] = s.z;

  result.m[1] = u.x;
  result.m[5] = u.y;
  result.m[9] = u.z;

  result.m[2] = -f.x;
  result.m[6] = -f.y;
  result.m[10] = -f.z;

  result.m[12] = -Vec3::dot(s, eye);
  result.m[13] = -Vec3::dot(u, eye);
  result.m[14] = Vec3::dot(f, eye);

  return result;
}

// Implémentation de MatrixStack

MatrixStack::MatrixStack() { stack.push_back(Mat4::identity()); }

void MatrixStack::push(const Mat4& mat) { stack.push_back(mat); }

Mat4 MatrixStack::pop() {
  if (stack.size() > 1) {
    Mat4 mat = stack.back();
    stack.pop_back();
    return mat;
  } else {
    return stack.back();
  }
}

Mat4 MatrixStack::top() const { return stack.back(); }
