#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

#include "BodyPart.h"

std::string loadShaderSource(const char* filepath) {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    std::cerr << "Erreur : impossible d'ouvrir le fichier shader " << filepath
              << std::endl;
    return "";
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::cout << "Shader chargé : " << filepath
            << std::endl;  // Afficher le chemin du shader chargé
  return buffer.str();
}

bool initGLFW() {
  if (!glfwInit()) {
    std::cerr << "Erreur lors de l'initialisation de GLFW" << std::endl;
    return false;
  }

  // Configurer la version d'OpenGL (ici 4.0)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  return true;
}

void setupOpenGL() {
  // Activer le test de profondeur pour que les objets plus proches soient
  // dessinés en premier
  glEnable(GL_DEPTH_TEST);

  // Définir la couleur de fond (gris foncé)
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void checkShaderCompilation(GLuint shader, std::string type) {
  GLint success;
  GLchar infoLog[1024];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
    std::cerr << "Erreur de compilation du " << type << " shader : " << infoLog
              << std::endl;
  }
}

void checkProgramLinking(GLuint program) {
  GLint success;
  GLchar infoLog[1024];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 1024, NULL, infoLog);
    std::cerr << "Erreur de linkage du programme shader : " << infoLog
              << std::endl;
  }
}

GLuint createShaderProgram() {
  // Charger les shaders
  std::string vertexCode = loadShaderSource("include/vertex_shader.glsl");
  std::string fragmentCode = loadShaderSource("include/fragment_shader.glsl");

  // Compiler le vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  const char* vertexSource = vertexCode.c_str();
  glShaderSource(vertexShader, 1, &vertexSource, nullptr);
  glCompileShader(vertexShader);
  checkShaderCompilation(vertexShader, "vertex");

  // Compiler le fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  const char* fragmentSource = fragmentCode.c_str();
  glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
  glCompileShader(fragmentShader);
  checkShaderCompilation(fragmentShader, "fragment");

  // Lier les shaders dans un programme
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  checkProgramLinking(shaderProgram);  // Ajout du débogage pour le linking

  // Nettoyer les shaders
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

// Variables globales pour le VAO et VBO
GLuint VAO, VBO;

// Tableau des positions des sommets d'un cube
// Les sommets du cube avec 36 entrées pour les triangles (12 triangles, 6
// faces)
float vertices[] = {
    // Face avant
    -0.5f,
    -0.5f,
    0.5f,
    0.5f,
    -0.5f,
    0.5f,
    0.5f,
    0.5f,
    0.5f,
    0.5f,
    0.5f,
    0.5f,
    -0.5f,
    0.5f,
    0.5f,
    -0.5f,
    -0.5f,
    0.5f,
    // Face arrière
    -0.5f,
    -0.5f,
    -0.5f,
    0.5f,
    -0.5f,
    -0.5f,
    0.5f,
    0.5f,
    -0.5f,
    0.5f,
    0.5f,
    -0.5f,
    -0.5f,
    0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    // Face gauche
    -0.5f,
    0.5f,
    0.5f,
    -0.5f,
    0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    0.5f,
    -0.5f,
    0.5f,
    0.5f,
    // Face droite
    0.5f,
    0.5f,
    0.5f,
    0.5f,
    0.5f,
    -0.5f,
    0.5f,
    -0.5f,
    -0.5f,
    0.5f,
    -0.5f,
    -0.5f,
    0.5f,
    -0.5f,
    0.5f,
    0.5f,
    0.5f,
    0.5f,
    // Face dessus
    -0.5f,
    0.5f,
    0.5f,
    0.5f,
    0.5f,
    0.5f,
    0.5f,
    0.5f,
    -0.5f,
    0.5f,
    0.5f,
    -0.5f,
    -0.5f,
    0.5f,
    -0.5f,
    -0.5f,
    0.5f,
    0.5f,
    // Face dessous
    -0.5f,
    -0.5f,
    0.5f,
    0.5f,
    -0.5f,
    0.5f,
    0.5f,
    -0.5f,
    -0.5f,
    0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    -0.5f,
    0.5f,
};

void setupCube() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Position des attributs du vertex
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

// Variables pour la gestion du temps
std::chrono::high_resolution_clock::time_point startTime =
    std::chrono::high_resolution_clock::now();

// Fonction de dessin pour le cube avec rotation sur plusieurs axes
void drawCube(GLuint shaderProgram, glm::mat4 model, const glm::mat4& view,
              const glm::mat4& projection) {
  glUseProgram(shaderProgram);

  GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
  GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
  GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

  // Calculer le temps écoulé pour animer la rotation
  auto currentTime = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float> elapsed =
      std::chrono::duration_cast<std::chrono::duration<float>>(currentTime -
                                                               startTime);
  //   float time = elapsed.count();  // Temps écoulé en secondes

  //   // Appliquer une rotation autour des axes Y et X
  //   model = glm::rotate(model, glm::radians(45.0f) * time,
  //                       glm::vec3(0.0f, 1.0f, 0.0f));  // Rotation autour de
  //                       Y
  //   model = glm::rotate(model, glm::radians(45.0f) * time,
  //                       glm::vec3(1.0f, 0.0f, 0.0f));  // Rotation autour de
  //                       X

  // Passer les matrices au shader
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);  // Dessiner le cube
  glBindVertexArray(0);
}

int main() {
  // Initialiser GLFW
  if (!initGLFW()) return -1;

  // Créer une fenêtre GLFW
  GLFWwindow* window = glfwCreateWindow(800, 600, "HumanGL", nullptr, nullptr);
  if (!window) {
    std::cerr << "Erreur lors de la création de la fenêtre GLFW" << std::endl;
    glfwTerminate();
    return -1;
  }

  // Faire de la fenêtre le contexte OpenGL actuel
  glfwMakeContextCurrent(window);

  // Initialiser GLEW
  if (glewInit() != GLEW_OK) {
    std::cerr << "Erreur lors de l'initialisation de GLEW" << std::endl;
    return -1;
  }

  setupOpenGL();
  setupCube();  // Préparer les données du cube

  // Créer le programme shader
  GLuint shaderProgram = createShaderProgram();

  // Matrices de vue et de projection
  glm::mat4 view =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 projection =
      glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

  // Création des matrices modèle pour chaque partie du corps
  glm::mat4 torsoModel =
      glm::scale(glm::mat4(1.0f),
                 glm::vec3(1.0f, 1.5f, 1.0f));  // Réduire la hauteur du torse

  glm::mat4 headModel = glm::translate(
      torsoModel, glm::vec3(0.0f, 1.8f, 0.0f));  // Rapprocher la tête du torse
  headModel = glm::scale(
      headModel, glm::vec3(0.7f, 0.7f,
                           0.7f));  // Augmenter légèrement la taille de la tête

  glm::mat4 leftArmModel = glm::translate(
      torsoModel, glm::vec3(-2.0f, 0.7f, 0.0f));  // Abaisser le bras gauche
  leftArmModel = glm::scale(leftArmModel,
                            glm::vec3(0.8f, 1.5f, 0.8f));  // Allonger les bras

  glm::mat4 rightArmModel = glm::translate(
      torsoModel, glm::vec3(2.0f, 0.7f, 0.0f));  // Abaisser le bras droit
  rightArmModel = glm::scale(rightArmModel,
                             glm::vec3(0.8f, 1.5f, 0.8f));  // Allonger les bras

  glm::mat4 leftLegModel = glm::translate(
      torsoModel, glm::vec3(-0.5f, -2.0f, 0.0f));  // Rapprocher la jambe gauche
  leftLegModel = glm::scale(
      leftLegModel,
      glm::vec3(0.8f, 1.2f, 0.8f));  // Allonger encore légèrement les jambes

  glm::mat4 rightLegModel = glm::translate(
      torsoModel, glm::vec3(0.5f, -2.0f, 0.0f));  // Rapprocher la jambe droite
  rightLegModel = glm::scale(
      rightLegModel,
      glm::vec3(0.8f, 1.2f, 0.8f));  // Allonger encore légèrement les jambes

  // Boucle de rendu
  while (!glfwWindowShouldClose(window)) {
    // Effacer l'écran et le buffer de profondeur
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Utiliser directement la matrice de base pour dessiner les parties
    drawCube(shaderProgram, torsoModel, view, projection);
    drawCube(shaderProgram, headModel, view, projection);
    drawCube(shaderProgram, leftArmModel, view, projection);
    drawCube(shaderProgram, rightArmModel, view, projection);
    drawCube(shaderProgram, leftLegModel, view, projection);
    drawCube(shaderProgram, rightLegModel, view, projection);

    // Afficher le rendu
    glfwSwapBuffers(window);

    // Gérer les événements
    glfwPollEvents();
  }

  // Nettoyage
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
