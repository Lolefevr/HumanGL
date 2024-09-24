#ifndef BODYPART_H
#define BODYPART_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>  // Pour les transformations
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

class BodyPart {
 public:
  std::string name;
  glm::vec3 size;
  glm::mat4 transform;
  std::vector<BodyPart*> children;

  // Constructeur
  BodyPart(const std::string& name, const glm::vec3& size);

  // Méthodes
  void addChild(BodyPart* child);
  void applyTransformation(const glm::mat4& parentTransform);
  void display(int depth = 0) const;

  // Nouvelles méthodes pour les transformations
  void rotate(float angle, const glm::vec3& axis);
  void translate(const glm::vec3& translation);
};

#endif
