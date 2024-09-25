#ifndef BODYPART_H
#define BODYPART_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>  // Pour les transformations
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

// --------------------[Classe BodyPart : gestion des parties du
// corps]--------------------

class BodyPart {
 public:
  // --------------------[Attributs : nom, taille, transformation,
  // enfants]--------------------

  std::string
      name;        // #REF001: Nom de la partie du corps (ex: "bras", "jambe").
  glm::vec3 size;  // #REF002: Dimensions de la partie du corps (largeur,
                   // hauteur, profondeur).
  glm::mat4 transform;  // #REF003: Matrice de transformation (position,
                        // échelle, rotation).
  std::vector<BodyPart*> children;  // #REF004: Parties enfants dans la
                                    // hiérarchie (ex: bras > avant-bras).

  // --------------------[Constructeur]--------------------

  BodyPart(const std::string& name,
           const glm::vec3& size);  // #REF005: Crée une partie du corps avec un
                                    // nom et des dimensions.

  // --------------------[Méthodes pour gérer les enfants et
  // transformations]--------------------

  void addChild(BodyPart* child);  // #REF006: Ajoute une partie enfant à la
                                   // partie courante.
  void applyTransformation(
      const glm::mat4&
          parentTransform);  // #REF007: Applique la transformation de la partie
                             // courante, puis à toutes ses enfants.
  void display(int depth = 0) const;  // #REF008: Affiche la hiérarchie des
                                      // parties du corps pour le débogage.

  // --------------------[Méthodes pour manipuler les
  // transformations]--------------------

  void rotate(float angle,
              const glm::vec3& axis);  // #REF009: Applique une rotation à la
                                       // partie du corps.
  void translate(
      const glm::vec3& translation);  // #REF010: Applique une translation à la
                                      // partie du corps.
};

#endif
