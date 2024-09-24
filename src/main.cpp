#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

std::string loadShaderSource(const char* filepath) {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    std::cerr << "Erreur : impossible d'ouvrir le fichier shader " << filepath
              << std::endl;
    return "";
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::cout << "Shader chargé : " << filepath << std::endl;
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
  checkProgramLinking(shaderProgram);

  // Nettoyer les shaders
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

// Variables globales pour le VAO et VBO
GLuint VAO, VBO;

// Tableau des positions des sommets d'un cube
float vertices[] = {
    // Positions
    // Face avant
    -0.5f, -0.5f, 0.5f,  // Bas gauche
    0.5f, -0.5f, 0.5f,   // Bas droit
    0.5f, 0.5f, 0.5f,    // Haut droit
    0.5f, 0.5f, 0.5f,    // Haut droit
    -0.5f, 0.5f, 0.5f,   // Haut gauche
    -0.5f, -0.5f, 0.5f,  // Bas gauche

    // Face arrière
    -0.5f, -0.5f, -0.5f,  // Bas gauche
    0.5f, -0.5f, -0.5f,   // Bas droit
    0.5f, 0.5f, -0.5f,    // Haut droit
    0.5f, 0.5f, -0.5f,    // Haut droit
    -0.5f, 0.5f, -0.5f,   // Haut gauche
    -0.5f, -0.5f, -0.5f,  // Bas gauche

    // Face gauche
    -0.5f, 0.5f, 0.5f,    // Haut avant
    -0.5f, 0.5f, -0.5f,   // Haut arrière
    -0.5f, -0.5f, -0.5f,  // Bas arrière
    -0.5f, -0.5f, -0.5f,  // Bas arrière
    -0.5f, -0.5f, 0.5f,   // Bas avant
    -0.5f, 0.5f, 0.5f,    // Haut avant

    // Face droite
    0.5f, 0.5f, 0.5f,    // Haut avant
    0.5f, 0.5f, -0.5f,   // Haut arrière
    0.5f, -0.5f, -0.5f,  // Bas arrière
    0.5f, -0.5f, -0.5f,  // Bas arrière
    0.5f, -0.5f, 0.5f,   // Bas avant
    0.5f, 0.5f, 0.5f,    // Haut avant

    // Face dessus
    -0.5f, 0.5f, 0.5f,   // Avant gauche
    0.5f, 0.5f, 0.5f,    // Avant droit
    0.5f, 0.5f, -0.5f,   // Arrière droit
    0.5f, 0.5f, -0.5f,   // Arrière droit
    -0.5f, 0.5f, -0.5f,  // Arrière gauche
    -0.5f, 0.5f, 0.5f,   // Avant gauche

    // Face dessous
    -0.5f, -0.5f, 0.5f,   // Avant gauche
    0.5f, -0.5f, 0.5f,    // Avant droit
    0.5f, -0.5f, -0.5f,   // Arrière droit
    0.5f, -0.5f, -0.5f,   // Arrière droit
    -0.5f, -0.5f, -0.5f,  // Arrière gauche
    -0.5f, -0.5f, 0.5f,   // Avant gauche
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

// Fonction de dessin pour le cube
void drawCube(GLuint shaderProgram, glm::mat4 model, const glm::mat4& view,
              const glm::mat4& projection) {
  glUseProgram(shaderProgram);

  GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
  GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
  GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

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
      glm::lookAt(glm::vec3(0.0f, 1.0f, 15.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 projection =
      glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

  // Boucle de rendu
  while (!glfwWindowShouldClose(window)) {
    // Effacer l'écran et le buffer de profondeur
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Appliquer une rotation globale pour une vue 3/4
    glm::mat4 globalRotation = glm::rotate(
        glm::mat4(1.0f), glm::radians(-20.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Animation de marche : bras et jambes alternent d'avant en arrière
    float time = glfwGetTime();  // Temps actuel

    // Calculer l'angle de balancement
    float swingAngle = glm::radians(30.0f) * sin(time * 2.0f);

    // Appliquer un décalage de phase de 180° entre les membres opposés
    float armAngleLeft = swingAngle;
    float armAngleRight = -swingAngle;
    float legAngleLeft = -swingAngle;
    float legAngleRight = swingAngle;

    // Torse
    glm::mat4 torsoModel = glm::mat4(1.0f);
    torsoModel = glm::translate(torsoModel, glm::vec3(0.0f, 1.0f, 0.0f));
    torsoModel = glm::scale(torsoModel, glm::vec3(1.0f, 2.0f, 0.5f));
    torsoModel = globalRotation * torsoModel;

    // Tête
    glm::mat4 headModel = glm::mat4(1.0f);
    headModel = glm::translate(headModel, glm::vec3(0.0f, 3.0f, 0.0f));
    headModel = glm::scale(headModel, glm::vec3(0.7f, 0.7f, 0.5f));
    headModel = globalRotation * headModel;

    // Longueurs des membres
    float armLength = 1.5f;
    float legLength = 2.0f;

    // Positions des articulations
    glm::vec3 leftShoulderPos = glm::vec3(-1.1f, 2.0f, 0.0f);
    glm::vec3 rightShoulderPos = glm::vec3(1.1f, 2.0f, 0.0f);
    glm::vec3 leftHipPos = glm::vec3(-0.5f, 0.0f, 0.0f);
    glm::vec3 rightHipPos = glm::vec3(0.5f, 0.0f, 0.0f);

    // Bras gauche
    glm::mat4 leftArmModel = glm::mat4(1.0f);
    leftArmModel = glm::translate(leftArmModel, leftShoulderPos);
    leftArmModel =
        glm::rotate(leftArmModel, armAngleLeft, glm::vec3(1.0f, 0.0f, 0.0f));
    leftArmModel =
        glm::translate(leftArmModel, glm::vec3(0.0f, -armLength / 2.0f, 0.0f));
    leftArmModel = glm::scale(leftArmModel, glm::vec3(0.3f, armLength, 0.3f));
    leftArmModel = globalRotation * leftArmModel;

    // Bras droit
    glm::mat4 rightArmModel = glm::mat4(1.0f);
    rightArmModel = glm::translate(rightArmModel, rightShoulderPos);
    rightArmModel =
        glm::rotate(rightArmModel, armAngleRight, glm::vec3(1.0f, 0.0f, 0.0f));
    rightArmModel =
        glm::translate(rightArmModel, glm::vec3(0.0f, -armLength / 2.0f, 0.0f));
    rightArmModel = glm::scale(rightArmModel, glm::vec3(0.3f, armLength, 0.3f));
    rightArmModel = globalRotation * rightArmModel;

    // Jambe gauche
    glm::mat4 leftLegModel = glm::mat4(1.0f);
    leftLegModel = glm::translate(leftLegModel, leftHipPos);
    leftLegModel =
        glm::rotate(leftLegModel, legAngleLeft, glm::vec3(1.0f, 0.0f, 0.0f));
    leftLegModel =
        glm::translate(leftLegModel, glm::vec3(0.0f, -legLength / 2.0f, 0.0f));
    leftLegModel = glm::scale(leftLegModel, glm::vec3(0.4f, legLength, 0.4f));
    leftLegModel = globalRotation * leftLegModel;

    // Jambe droite
    glm::mat4 rightLegModel = glm::mat4(1.0f);
    rightLegModel = glm::translate(rightLegModel, rightHipPos);
    rightLegModel =
        glm::rotate(rightLegModel, legAngleRight, glm::vec3(1.0f, 0.0f, 0.0f));
    rightLegModel =
        glm::translate(rightLegModel, glm::vec3(0.0f, -legLength / 2.0f, 0.0f));
    rightLegModel = glm::scale(rightLegModel, glm::vec3(0.4f, legLength, 0.4f));
    rightLegModel = globalRotation * rightLegModel;

    // Dessiner les parties du corps
    drawCube(shaderProgram, torsoModel, view, projection);     // Torse
    drawCube(shaderProgram, headModel, view, projection);      // Tête
    drawCube(shaderProgram, leftArmModel, view, projection);   // Bras gauche
    drawCube(shaderProgram, rightArmModel, view, projection);  // Bras droit
    drawCube(shaderProgram, leftLegModel, view, projection);   // Jambe gauche
    drawCube(shaderProgram, rightLegModel, view, projection);  // Jambe droite

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
