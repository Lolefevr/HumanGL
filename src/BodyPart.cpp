#include "BodyPart.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Constructeur
BodyPart::BodyPart(const std::string& name, const glm::vec3& size)
    : name(name), size(size), transform(glm::mat4(1.0f)) {}

// Ajouter une partie enfant
void BodyPart::addChild(BodyPart* child) { children.push_back(child); }

// Appliquer la transformation
void BodyPart::applyTransformation(const glm::mat4& parentTransform) {
  transform = parentTransform * transform;
  for (auto& child : children) {
    child->applyTransformation(transform);
  }
}

// Appliquer une rotation à la partie du corps
void BodyPart::rotate(float angle, const glm::vec3& axis) {
  transform = glm::rotate(transform, glm::radians(angle), axis);
}

// Appliquer une translation à la partie du corps
void BodyPart::translate(const glm::vec3& translation) {
  transform = glm::translate(transform, translation);
}

// Afficher la structure du squelette (pour debug)
void BodyPart::display(int depth) const {
  for (int i = 0; i < depth; i++) std::cout << "--";
  std::cout << name << " (size: " << size.x << ", " << size.y << ", " << size.z
            << ")\n";
  for (const auto& child : children) {
    child->display(depth + 1);
  }
}
