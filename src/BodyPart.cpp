#include "BodyPart.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// --------------------[Constructeur BodyPart]--------------------

BodyPart::BodyPart(const std::string& name, const glm::vec3& size)
    : name(name),
      size(size),
      transform(glm::mat4(1.0f)) {
}  // #REF001: Initialisation d'une partie du corps avec un nom, une taille et
   // une matrice identité.

// --------------------[Ajout d'une partie enfant]--------------------

void BodyPart::addChild(
    BodyPart* child) {  // #REF002: Ajoute une partie enfant dans la hiérarchie
                        // de cette partie du corps.
  children.push_back(child);
}

// --------------------[Application de la transformation]--------------------

void BodyPart::applyTransformation(
    const glm::mat4&
        parentTransform) {  // #REF003: Applique une transformation à la partie
                            // du corps courante et à toutes ses enfants.
  transform = parentTransform * transform;
  for (auto& child : children) {
    child->applyTransformation(transform);
  }
}

// --------------------[Transformation : rotation]--------------------

void BodyPart::rotate(
    float angle,
    const glm::vec3& axis) {  // #REF004: Applique une rotation autour d'un axe
                              // à la partie du corps.
  transform = glm::rotate(transform, glm::radians(angle), axis);
}

// --------------------[Transformation : translation]--------------------

void BodyPart::translate(
    const glm::vec3& translation) {  // #REF005: Applique une translation à la
                                     // partie du corps.
  transform = glm::translate(transform, translation);
}

// --------------------[Affichage de la structure hiérarchique
// (debug)]--------------------

void BodyPart::display(
    int depth) const {  // #REF006: Affiche la hiérarchie des parties du corps
                        // avec indentation pour chaque niveau de profondeur.
  for (int i = 0; i < depth; i++) std::cout << "--";
  std::cout << name << " (size: " << size.x << ", " << size.y << ", " << size.z
            << ")\n";
  for (const auto& child : children) {
    child->display(depth + 1);
  }
}
