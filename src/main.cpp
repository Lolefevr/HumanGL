#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <cmath>
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

// Variables pour la taille de la fenêtre
int windowWidth = 800;
int windowHeight = 600;

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

// Callback pour gérer le redimensionnement de la fenêtre
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  windowWidth = width;
  windowHeight = height;
  glViewport(0, 0, width, height);
}

int main() {
  // Initialiser GLFW
  if (!initGLFW()) return -1;

  // Créer une fenêtre GLFW
  GLFWwindow* window =
      glfwCreateWindow(windowWidth, windowHeight, "HumanGL", nullptr, nullptr);
  if (!window) {
    std::cerr << "Erreur lors de la création de la fenêtre GLFW" << std::endl;
    glfwTerminate();
    return -1;
  }

  // Faire de la fenêtre le contexte OpenGL actuel
  glfwMakeContextCurrent(window);

  // Définir le callback pour le redimensionnement de la fenêtre
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // Initialiser GLEW
  if (glewInit() != GLEW_OK) {
    std::cerr << "Erreur lors de l'initialisation de GLEW" << std::endl;
    return -1;
  }

  setupOpenGL();
  setupCube();  // Préparer les données du cube

  // Créer le programme shader
  GLuint shaderProgram = createShaderProgram();

  // Longueurs des segments (proportions humaines réalistes)
  float headHeight = 1.0f;
  float torsoHeight = 3.0f;
  float upperArmLength = 1.0f;
  float forearmLength = 1.0f;
  float handLength = 0.5f;
  float thighLength = 2.0f;
  float shinLength = 2.0f;
  float footLength = 0.5f;

  // Calculer la hauteur totale du personnage
  float totalHeight = footLength + shinLength + thighLength + torsoHeight +
                      headHeight;  // 8.5 unités

  // Positions des articulations (par rapport au torse)
  glm::vec3 leftShoulderPos = glm::vec3(-1.0f, torsoHeight - 0.5f, 0.0f);
  glm::vec3 rightShoulderPos = glm::vec3(1.0f, torsoHeight - 0.5f, 0.0f);
  glm::vec3 leftHipPos = glm::vec3(-0.5f, 0.0f, 0.0f);
  glm::vec3 rightHipPos = glm::vec3(0.5f, 0.0f, 0.0f);

  // Distance de la caméra en fonction de la taille du personnage
  float cameraDistance = totalHeight * 2.5f;

  // Champ de vision (FOV)
  float fov = 60.0f;

  // Boucle de rendu
  while (!glfwWindowShouldClose(window)) {
    // Effacer l'écran et le buffer de profondeur
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Temps actuel
    float time = glfwGetTime();

    // Animation de marche
    float walkCycle = fmod(time, 4.0f);  // Cycle de 4 secondes
    float walkPhase = (walkCycle / 4.0f) * 2.0f * glm::pi<float>();

    // Déplacement horizontal pour simuler la marche
    float horizontalMovement = sin(walkPhase) * 2.0f;

    // Mouvement vertical pour simuler le rebond de la marche
    float verticalMovement =
        fabs(sin(walkPhase)) * 0.05f;  // Réduit pour limiter le rebond

    // Balancement des bras et des jambes
    float armSwing = sin(walkPhase) * glm::radians(30.0f);
    float legSwing = sin(walkPhase) * glm::radians(30.0f);

    // Angles pour les coudes et les genoux
    float elbowAngle =
        glm::radians(15.0f) + fabs(sin(walkPhase)) * glm::radians(30.0f);
    float kneeAngle = fabs(sin(walkPhase)) * glm::radians(45.0f);

    // Légère rotation du torse
    float torsoRotation = sin(walkPhase) * glm::radians(5.0f);

    // Mouvement de la tête
    float headTilt = sin(walkPhase + glm::pi<float>()) * glm::radians(5.0f);

    // Appliquer une rotation globale pour une vue 3/4
    glm::mat4 globalRotation = glm::rotate(
        glm::mat4(1.0f), glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Transformation globale (position du personnage)
    glm::mat4 globalTransform = glm::translate(
        glm::mat4(1.0f), glm::vec3(horizontalMovement, verticalMovement, 0.0f));

    // Matrice de vue (ajustée pour suivre le personnage)
    glm::mat4 view = glm::lookAt(
        glm::vec3(horizontalMovement, totalHeight / 2.0f + 0.2f,
                  cameraDistance),
        glm::vec3(horizontalMovement, totalHeight / 2.0f + 0.2f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    // Matrice de projection (ajustée pour le rapport d'aspect)
    float aspectRatio =
        static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    glm::mat4 projection =
        glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 1000.0f);

    // Torse
    glm::mat4 torsoModel = glm::mat4(1.0f);
    torsoModel = globalTransform * globalRotation * torsoModel;
    torsoModel =
        glm::rotate(torsoModel, torsoRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    torsoModel = glm::translate(
        torsoModel,
        glm::vec3(0.0f,
                  thighLength + shinLength + footLength + torsoHeight / 2.0f,
                  0.0f));
    torsoModel = glm::scale(torsoModel, glm::vec3(1.5f, torsoHeight, 0.7f));

    // Tête
    glm::mat4 headModel = glm::mat4(1.0f);
    headModel = torsoModel;
    headModel = glm::translate(
        headModel,
        glm::vec3(0.0f, torsoHeight / 2.0f + headHeight / 2.0f, 0.0f));
    headModel = glm::rotate(headModel, headTilt, glm::vec3(0.0f, 1.0f, 0.0f));
    headModel = glm::scale(headModel, glm::vec3(0.9f, headHeight, 0.8f));

    // Bras gauche (haut)
    glm::mat4 leftUpperArmModel = glm::mat4(1.0f);
    leftUpperArmModel = torsoModel;
    leftUpperArmModel = glm::translate(
        leftUpperArmModel,
        leftShoulderPos - glm::vec3(0.0f, torsoHeight / 2.0f, 0.0f));
    leftUpperArmModel =
        glm::rotate(leftUpperArmModel, -armSwing, glm::vec3(1.0f, 0.0f, 0.0f));
    leftUpperArmModel = glm::translate(
        leftUpperArmModel, glm::vec3(0.0f, -upperArmLength / 2.0f, 0.0f));
    leftUpperArmModel =
        glm::scale(leftUpperArmModel, glm::vec3(0.4f, upperArmLength, 0.4f));

    // Avant-bras gauche
    glm::mat4 leftForearmModel = glm::mat4(1.0f);
    leftForearmModel = leftUpperArmModel;
    leftForearmModel = glm::translate(
        leftForearmModel, glm::vec3(0.0f, -upperArmLength / 2.0f, 0.0f));
    leftForearmModel =
        glm::rotate(leftForearmModel, -elbowAngle, glm::vec3(1.0f, 0.0f, 0.0f));
    leftForearmModel = glm::translate(
        leftForearmModel, glm::vec3(0.0f, -forearmLength / 2.0f, 0.0f));
    leftForearmModel =
        glm::scale(leftForearmModel, glm::vec3(0.35f, forearmLength, 0.35f));

    // Main gauche
    glm::mat4 leftHandModel = glm::mat4(1.0f);
    leftHandModel = leftForearmModel;
    leftHandModel = glm::translate(
        leftHandModel, glm::vec3(0.0f, -forearmLength / 2.0f, 0.0f));
    leftHandModel =
        glm::scale(leftHandModel, glm::vec3(0.3f, handLength, 0.3f));

    // Bras droit (haut)
    glm::mat4 rightUpperArmModel = glm::mat4(1.0f);
    rightUpperArmModel = torsoModel;
    rightUpperArmModel = glm::translate(
        rightUpperArmModel,
        rightShoulderPos - glm::vec3(0.0f, torsoHeight / 2.0f, 0.0f));
    rightUpperArmModel =
        glm::rotate(rightUpperArmModel, armSwing, glm::vec3(1.0f, 0.0f, 0.0f));
    rightUpperArmModel = glm::translate(
        rightUpperArmModel, glm::vec3(0.0f, -upperArmLength / 2.0f, 0.0f));
    rightUpperArmModel =
        glm::scale(rightUpperArmModel, glm::vec3(0.4f, upperArmLength, 0.4f));

    // Avant-bras droit
    glm::mat4 rightForearmModel = glm::mat4(1.0f);
    rightForearmModel = rightUpperArmModel;
    rightForearmModel = glm::translate(
        rightForearmModel, glm::vec3(0.0f, -upperArmLength / 2.0f, 0.0f));
    rightForearmModel =
        glm::rotate(rightForearmModel, elbowAngle, glm::vec3(1.0f, 0.0f, 0.0f));
    rightForearmModel = glm::translate(
        rightForearmModel, glm::vec3(0.0f, -forearmLength / 2.0f, 0.0f));
    rightForearmModel =
        glm::scale(rightForearmModel, glm::vec3(0.35f, forearmLength, 0.35f));

    // Main droite
    glm::mat4 rightHandModel = glm::mat4(1.0f);
    rightHandModel = rightForearmModel;
    rightHandModel = glm::translate(
        rightHandModel, glm::vec3(0.0f, -forearmLength / 2.0f, 0.0f));
    rightHandModel =
        glm::scale(rightHandModel, glm::vec3(0.3f, handLength, 0.3f));

    // Cuisse gauche
    glm::mat4 leftThighModel = glm::mat4(1.0f);
    leftThighModel = globalTransform * globalRotation;
    leftThighModel =
        glm::rotate(leftThighModel, torsoRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    leftThighModel = glm::translate(
        leftThighModel,
        leftHipPos +
            glm::vec3(0.0f,
                      thighLength + shinLength + footLength - verticalMovement,
                      0.0f));
    leftThighModel =
        glm::rotate(leftThighModel, legSwing, glm::vec3(1.0f, 0.0f, 0.0f));
    leftThighModel = glm::translate(leftThighModel,
                                    glm::vec3(0.0f, -thighLength / 2.0f, 0.0f));
    leftThighModel =
        glm::scale(leftThighModel, glm::vec3(0.6f, thighLength, 0.6f));

    // Tibia gauche
    glm::mat4 leftShinModel = glm::mat4(1.0f);
    leftShinModel = leftThighModel;
    leftShinModel = glm::translate(leftShinModel,
                                   glm::vec3(0.0f, -thighLength / 2.0f, 0.0f));
    leftShinModel =
        glm::rotate(leftShinModel, kneeAngle, glm::vec3(1.0f, 0.0f, 0.0f));
    leftShinModel = glm::translate(leftShinModel,
                                   glm::vec3(0.0f, -shinLength / 2.0f, 0.0f));
    leftShinModel =
        glm::scale(leftShinModel, glm::vec3(0.5f, shinLength, 0.5f));

    // Pied gauche
    glm::mat4 leftFootModel = glm::mat4(1.0f);
    leftFootModel = leftShinModel;
    leftFootModel = glm::translate(leftFootModel,
                                   glm::vec3(0.0f, -shinLength / 2.0f, 0.2f));
    leftFootModel = glm::rotate(leftFootModel, glm::radians(-10.0f),
                                glm::vec3(1.0f, 0.0f, 0.0f));
    leftFootModel =
        glm::scale(leftFootModel, glm::vec3(0.5f, footLength, 1.0f));

    // Cuisse droite
    glm::mat4 rightThighModel = glm::mat4(1.0f);
    rightThighModel = globalTransform * globalRotation;
    rightThighModel = glm::rotate(rightThighModel, torsoRotation,
                                  glm::vec3(0.0f, 1.0f, 0.0f));
    rightThighModel = glm::translate(
        rightThighModel,
        rightHipPos +
            glm::vec3(0.0f,
                      thighLength + shinLength + footLength - verticalMovement,
                      0.0f));
    rightThighModel =
        glm::rotate(rightThighModel, -legSwing, glm::vec3(1.0f, 0.0f, 0.0f));
    rightThighModel = glm::translate(
        rightThighModel, glm::vec3(0.0f, -thighLength / 2.0f, 0.0f));
    rightThighModel =
        glm::scale(rightThighModel, glm::vec3(0.6f, thighLength, 0.6f));

    // Tibia droit
    glm::mat4 rightShinModel = glm::mat4(1.0f);
    rightShinModel = rightThighModel;
    rightShinModel = glm::translate(rightShinModel,
                                    glm::vec3(0.0f, -thighLength / 2.0f, 0.0f));
    rightShinModel =
        glm::rotate(rightShinModel, kneeAngle, glm::vec3(1.0f, 0.0f, 0.0f));
    rightShinModel = glm::translate(rightShinModel,
                                    glm::vec3(0.0f, -shinLength / 2.0f, 0.0f));
    rightShinModel =
        glm::scale(rightShinModel, glm::vec3(0.5f, shinLength, 0.5f));

    // Pied droit
    glm::mat4 rightFootModel = glm::mat4(1.0f);
    rightFootModel = rightShinModel;
    rightFootModel = glm::translate(rightFootModel,
                                    glm::vec3(0.0f, -shinLength / 2.0f, 0.2f));
    rightFootModel = glm::rotate(rightFootModel, glm::radians(-10.0f),
                                 glm::vec3(1.0f, 0.0f, 0.0f));
    rightFootModel =
        glm::scale(rightFootModel, glm::vec3(0.5f, footLength, 1.0f));

    // Dessiner les parties du corps
    drawCube(shaderProgram, torsoModel, view, projection);  // Torse
    drawCube(shaderProgram, headModel, view, projection);   // Tête
    drawCube(shaderProgram, leftUpperArmModel, view,
             projection);  // Bras gauche (haut)
    drawCube(shaderProgram, leftForearmModel, view,
             projection);  // Avant-bras gauche
    drawCube(shaderProgram, leftHandModel, view, projection);  // Main gauche
    drawCube(shaderProgram, rightUpperArmModel, view,
             projection);  // Bras droit (haut)
    drawCube(shaderProgram, rightForearmModel, view,
             projection);  // Avant-bras droit
    drawCube(shaderProgram, rightHandModel, view, projection);  // Main droite
    drawCube(shaderProgram, leftThighModel, view, projection);  // Cuisse gauche
    drawCube(shaderProgram, leftShinModel, view, projection);   // Tibia gauche
    drawCube(shaderProgram, leftFootModel, view, projection);   // Pied gauche
    drawCube(shaderProgram, rightThighModel, view,
             projection);                                       // Cuisse droite
    drawCube(shaderProgram, rightShinModel, view, projection);  // Tibia droit
    drawCube(shaderProgram, rightFootModel, view, projection);  // Pied droit

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
