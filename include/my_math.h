#ifndef MY_MATH_H
#define MY_MATH_H

#include <cmath>
#include <vector>

// --------------------[Structure Vec3 : gestion des vecteurs
// 3D]--------------------

struct Vec3 {
  float x, y, z;  // #REF001: Composantes du vecteur 3D (x, y, z).

  // --------------------[Constructeurs]--------------------

  Vec3();  // #REF002: Constructeur par défaut (initialise le vecteur à zéro).
  Vec3(float x, float y, float z);  // #REF003: Constructeur pour initialiser un
                                    // vecteur avec des valeurs spécifiques.

  // --------------------[Opérations sur les vecteurs]--------------------

  Vec3 operator+(
      const Vec3& other) const;  // #REF004: Addition de deux vecteurs.
  Vec3 operator-(
      const Vec3& other) const;  // #REF005: Soustraction de deux vecteurs.
  Vec3 operator*(float scalar)
      const;  // #REF006: Multiplication d'un vecteur par un scalaire.
  static Vec3 cross(
      const Vec3& a,
      const Vec3& b);  // #REF007: Produit vectoriel entre deux vecteurs.
  static float dot(
      const Vec3& a,
      const Vec3& b);  // #REF008: Produit scalaire entre deux vecteurs.
  static Vec3 normalize(const Vec3& v);  // #REF009: Normalise un vecteur (le
                                         // ramène à une longueur de 1).
};

// --------------------[Structure Mat4 : gestion des matrices
// 4x4]--------------------

struct Mat4 {
  float m[16];  // #REF010: Tableau de 16 valeurs représentant les éléments de
                // la matrice.

  // --------------------[Constructeurs et méthodes de base]--------------------

  Mat4();  // #REF011: Constructeur par défaut (initialise une matrice vide).
  static Mat4 identity();  // #REF012: Retourne une matrice identité (aucune
                           // transformation).

  // --------------------[Transformations sur les matrices]--------------------

  static Mat4 translate(
      const Mat4& mat,
      const Vec3& vec);  // #REF013: Applique une translation à une matrice.
  static Mat4 rotate(const Mat4& mat, float angle,
                     const Vec3& axis);  // #REF014: Applique une rotation
                                         // autour d'un axe à une matrice.
  static Mat4 scale(
      const Mat4& mat,
      const Vec3& vec);  // #REF015: Applique une échelle à une matrice.
  static Mat4 perspective(
      float fov, float aspect, float near,
      float far);  // #REF016: Crée une matrice de projection perspective.
  static Mat4 lookAt(
      const Vec3& eye, const Vec3& center,
      const Vec3& up);  // #REF017: Crée une matrice pour une vue caméra.
  static Mat4 multiply(const Mat4& a,
                       const Mat4& b);  // #REF018: Multiplie deux matrices 4x4.
};

// --------------------[Classe MatrixStack : gestion d'une pile de
// matrices]--------------------

class MatrixStack {
 public:
  MatrixStack();  // #REF019: Constructeur, initialise une pile avec la matrice
                  // identité.
  void push(
      const Mat4& mat);  // #REF020: Ajoute une matrice au sommet de la pile.
  Mat4
  pop();  // #REF021: Retire la matrice du sommet de la pile et la retourne.
  Mat4 top() const;  // #REF022: Retourne la matrice au sommet de la pile sans
                     // la retirer.

 private:
  std::vector<Mat4>
      stack;  // #REF023: Vecteur de matrices qui représente la pile.
};

#endif  // MY_MATH_H
