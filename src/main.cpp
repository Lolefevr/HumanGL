// Inclure les en-têtes standard
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// Inclure GLEW
#include <GL/glew.h>

// Inclure GLFW
#include <GLFW/glfw3.h>

// Inclure GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Définition de la largeur et de la hauteur de la fenêtre
const GLuint WIDTH = 800, HEIGHT = 600;

// Variables globales pour le VAO et le VBO
GLuint VAO, VBO;

// Variables pour la taille de la fenêtre
int windowWidth = WIDTH;
int windowHeight = HEIGHT;

// Fonction pour charger le code du shader depuis un fichier
std::string loadShaderSource(const char* filepath) {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    std::cerr << "Erreur : impossible d'ouvrir le fichier shader " << filepath
              << std::endl;
    return "";
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

// Fonction pour initialiser GLFW
bool initGLFW() {
  if (!glfwInit()) {
    std::cerr << "Erreur lors de l'initialisation de GLFW" << std::endl;
    return false;
  }

  // Configurer la version d'OpenGL (ici 3.3)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  return true;
}

// Fonction pour configurer OpenGL
void setupOpenGL() {
  // Activer le test de profondeur pour que les objets plus proches soient
  // dessinés en premier
  glEnable(GL_DEPTH_TEST);

  // Définir la couleur de fond (gris foncé)
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

// Fonction pour vérifier la compilation des shaders
void checkShaderCompilation(GLuint shader, std::string type) {
  GLint success;
  GLchar infoLog[1024];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
    std::cerr << "Erreur de compilation du shader " << type << " : " << infoLog
              << std::endl;
  }
}

// Fonction pour vérifier le linkage du programme shader
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

// Fonction pour créer le programme shader
GLuint createShaderProgram() {
  // Charger les shaders
  std::string vertexCode = loadShaderSource("include/vertex_shader.glsl");
  std::string fragmentCode = loadShaderSource("include/fragment_shader.glsl");

  // Vérifier que les shaders ont été chargés correctement
  if (vertexCode.empty() || fragmentCode.empty()) {
    std::cerr << "Erreur : le code des shaders est vide." << std::endl;
    return 0;
  }

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

  // Nettoyer les shaders car ils ne sont plus nécessaires après le linkage
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

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

// Fonction pour préparer les données du cube
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

// Fonction d'interpolation linéaire
float lerp(float a, float b, float t) { return a + t * (b - a); }

// Fonction smoothstep pour une interpolation plus douce
float smoothstep(float edge0, float edge1, float x) {
  x = glm::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
  return x * x * (3 - 2 * x);
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
  glewExperimental = GL_TRUE;  // Activer les fonctionnalités modernes
  if (glewInit() != GLEW_OK) {
    std::cerr << "Erreur lors de l'initialisation de GLEW" << std::endl;
    return -1;
  }

  setupOpenGL();
  setupCube();  // Préparer les données du cube

  // Créer le programme shader
  GLuint shaderProgram = createShaderProgram();
  if (shaderProgram == 0) {
    std::cerr << "Erreur lors de la création du programme shader" << std::endl;
    return -1;
  }

  // Longueurs des segments (proportions humaines réalistes)
  float headHeight = 0.6f;   // Réduit pour une tête plus petite
  float neckHeight = 0.02f;  // Réduit pour rapprocher la tête du buste
  float torsoHeight = 2.2f;
  float pelvisHeight = 1.0f;
  float upperArmLength = 1.0f;
  float forearmLength = 1.0f;
  float thighLength = 1.5f;
  float shinLength = 1.3f;

  // Calculer la hauteur totale du personnage
  float totalHeight = shinLength + thighLength + pelvisHeight + torsoHeight +
                      neckHeight + headHeight;

  // Positions des articulations (par rapport au torse)
  glm::vec3 leftShoulderPos = glm::vec3(-1.0f, torsoHeight / 2.0f - 0.2f, 0.0f);
  glm::vec3 rightShoulderPos = glm::vec3(1.0f, torsoHeight / 2.0f - 0.2f, 0.0f);
  glm::vec3 leftHipPos =
      glm::vec3(-0.5f, -torsoHeight / 2.0f - pelvisHeight / 2.0f + 0.4f, 0.0f);
  glm::vec3 rightHipPos =
      glm::vec3(0.5f, -torsoHeight / 2.0f - pelvisHeight / 2.0f + 0.4f, 0.0f);

  // Distance de la caméra en fonction de la taille du personnage
  float cameraDistance = totalHeight * 3.0f;

  // Champ de vision (FOV)
  float fov = 45.0f;

  // Déclarer les états d'animation
  enum AnimationState { WALKING, JUMPING, FROM_JUMP_TO_IDLE, IDLE };
  AnimationState currentState = WALKING;
  float stateTime = 0.0f;
  float time = glfwGetTime();

  // Durées des états en secondes
  const float WALK_DURATION = 3.0f;
  const float JUMP_DURATION = 2.0f;
  const float JUMP_TO_IDLE_DURATION = 2.0f;
  const float IDLE_DURATION = 2.5f;

  // Variables pour l'animation de saut
  float jumpHeight = 5.0f;  // Hauteur maximale du saut

  // Variables pour l'interpolation
  float prevKneeAngle = 0.0f;
  float targetKneeAngle = 0.0f;
  float prevArmSwing = 0.0f;
  float targetArmSwing = 0.0f;
  float prevVerticalMovement = 0.0f;
  float targetVerticalMovement = 0.0f;

  // Boucle de rendu
  while (!glfwWindowShouldClose(window)) {
    // Effacer l'écran et le buffer de profondeur
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Temps actuel
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - time;
    time = currentTime;
    stateTime += deltaTime;

    // Variables pour l'animation
    float verticalMovement = 0.0f;
    float armSwing = 0.0f;
    float legSwing = 0.0f;
    float elbowAngle = 0.0f;
    float kneeAngle = 0.0f;
    float horizontalMovement = 0.0f;

    // Gestion des transitions entre les états
    switch (currentState) {
      case WALKING:
        if (stateTime >= WALK_DURATION) {
          currentState = JUMPING;
          stateTime = 0.0f;
        }
        break;
      case JUMPING:
        if (stateTime >= JUMP_DURATION) {
          // Enregistrer les valeurs actuelles pour l'interpolation
          prevKneeAngle = kneeAngle;
          prevArmSwing = armSwing;
          prevVerticalMovement = verticalMovement;

          // Définir les cibles pour la transition
          targetKneeAngle = 0.0f;
          targetArmSwing = 0.0f;
          targetVerticalMovement = 0.0f;

          currentState = FROM_JUMP_TO_IDLE;
          stateTime = 0.0f;
        }
        break;
      case FROM_JUMP_TO_IDLE:
        if (stateTime >= JUMP_TO_IDLE_DURATION) {
          currentState = IDLE;
          stateTime = 0.0f;
        }
        break;
      case IDLE:
        if (stateTime >= IDLE_DURATION) {
          currentState = WALKING;
          stateTime = 0.0f;
        }
        break;
    }

    // Calcul des animations en fonction de l'état
    if (currentState == WALKING) {
      // Animation de marche
      float walkCycle = fmod(stateTime, WALK_DURATION) / WALK_DURATION;
      float walkPhase = walkCycle * 2.0f * glm::pi<float>();

      // Déplacement vertical pour simuler le rebond de la marche
      verticalMovement = fabs(sin(walkPhase)) * 0.3f;

      // Balancement des bras et des jambes
      armSwing = sin(walkPhase) * glm::radians(30.0f);
      legSwing = sin(walkPhase) * glm::radians(30.0f);

      // Angles des coudes et genoux
      elbowAngle =
          glm::radians(15.0f) + fabs(sin(walkPhase)) * glm::radians(30.0f);
      kneeAngle = fabs(sin(walkPhase)) * glm::radians(45.0f);

    } else if (currentState == JUMPING) {
      // Animation de saut avec anticipation
      float jumpProgress = stateTime / JUMP_DURATION;

      if (jumpProgress < 0.2f) {
        // Phase d'anticipation (crouch)
        float crouchAmount = sin(jumpProgress / 0.2f * glm::half_pi<float>());
        verticalMovement = -crouchAmount * 0.5f;
        kneeAngle = crouchAmount * glm::radians(60.0f);
        armSwing = -crouchAmount * glm::radians(30.0f);
      } else if (jumpProgress < 0.8f) {
        // Phase d'ascension et descente
        float jumpPhase = (jumpProgress - 0.2f) / 0.6f * glm::pi<float>();
        verticalMovement = sin(jumpPhase) * jumpHeight;
        kneeAngle = (1.0f - sin(jumpPhase)) * glm::radians(30.0f);
        armSwing = sin(jumpPhase) * glm::radians(20.0f);
      } else {
        // Phase d'atterrissage
        float landProgress = (jumpProgress - 0.8f) / 0.2f;
        float crouchAmount = sin(landProgress * glm::half_pi<float>());
        verticalMovement = -crouchAmount * 0.5f;
        kneeAngle = crouchAmount * glm::radians(60.0f);
        armSwing = -crouchAmount * glm::radians(30.0f);
      }

      elbowAngle = glm::radians(10.0f);

    } else if (currentState == FROM_JUMP_TO_IDLE) {
      // Transition du saut à l'état immobile
      float t = stateTime / JUMP_TO_IDLE_DURATION;
      t = smoothstep(0.0f, 3.0f, t);
      kneeAngle = lerp(prevKneeAngle, targetKneeAngle, t);
      armSwing = lerp(prevArmSwing, targetArmSwing, t);
      verticalMovement = lerp(prevVerticalMovement, targetVerticalMovement, t);
      elbowAngle = glm::radians(15.0f);
    } else if (currentState == IDLE) {
      // Posture immobile
      verticalMovement = 0.0f;
      armSwing = 0.0f;
      legSwing = 0.0f;
      elbowAngle = glm::radians(15.0f);
      kneeAngle = 0.0f;
    }

    // Légère rotation du torse
    float torsoRotation = sin(time) * glm::radians(5.0f);

    // Mouvement de la tête
    float headTilt = sin(time + glm::pi<float>()) * glm::radians(5.0f);

    // Rotation lente du personnage sur lui-même
    float selfRotationAngle = time * glm::radians(8.0f);

    // Appliquer une rotation globale pour une vue 3/4 et la rotation sur
    // lui-même
    glm::mat4 globalRotation = glm::rotate(glm::mat4(1.0f), selfRotationAngle,
                                           glm::vec3(0.0f, 1.0f, 0.0f));
    globalRotation = glm::rotate(globalRotation, glm::radians(-30.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));

    // Transformation globale (position du personnage)
    glm::mat4 globalTransform = glm::translate(
        glm::mat4(1.0f), glm::vec3(horizontalMovement, verticalMovement, 0.0f));

    // Matrice de vue (ajustée pour suivre le personnage)
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, totalHeight / 2.0f, cameraDistance),
        glm::vec3(0.0f, totalHeight / 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

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
        torsoModel, glm::vec3(0.0f,
                              thighLength + shinLength + pelvisHeight +
                                  torsoHeight / 2.0f - 0.4f,
                              0.0f));
    torsoModel = glm::scale(torsoModel, glm::vec3(1.5f, torsoHeight, 0.7f));

    // Pelvis
    glm::mat4 pelvisModel = glm::mat4(1.0f);
    pelvisModel = globalTransform * globalRotation;
    pelvisModel =
        glm::rotate(pelvisModel, torsoRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    pelvisModel = glm::translate(
        pelvisModel,
        glm::vec3(0.0f, thighLength + shinLength + pelvisHeight / 2.0f - 0.4f,
                  0.0f));
    pelvisModel = glm::scale(pelvisModel, glm::vec3(1.5f, pelvisHeight, 0.8f));

    // Tête
    glm::mat4 headModel = glm::mat4(1.0f);
    headModel = torsoModel;
    headModel = glm::translate(
        headModel,
        glm::vec3(0.0f,
                  torsoHeight / 2.0f + neckHeight + headHeight / 2.0f - 0.15f,
                  0.0f));
    headModel = glm::rotate(headModel, headTilt, glm::vec3(0.0f, 1.0f, 0.0f));
    headModel = glm::scale(headModel, glm::vec3(0.6f, headHeight, 0.5f));

    // Mouvement latéral des bras
    float armSideSwing = sin(time) * glm::radians(5.0f);

    // Bras gauche (haut)
    glm::mat4 leftUpperArmModel = glm::mat4(1.0f);
    leftUpperArmModel = torsoModel;
    leftUpperArmModel = glm::translate(leftUpperArmModel, leftShoulderPos);
    leftUpperArmModel = glm::rotate(leftUpperArmModel, armSideSwing,
                                    glm::vec3(0.0f, 0.0f, 1.0f));
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

    // Bras droit (haut)
    glm::mat4 rightUpperArmModel = glm::mat4(1.0f);
    rightUpperArmModel = torsoModel;
    rightUpperArmModel = glm::translate(rightUpperArmModel, rightShoulderPos);
    rightUpperArmModel = glm::rotate(rightUpperArmModel, -armSideSwing,
                                     glm::vec3(0.0f, 0.0f, 1.0f));
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

    // Cuisse gauche
    glm::mat4 leftThighModel = glm::mat4(1.0f);
    leftThighModel = pelvisModel;
    leftThighModel = glm::translate(leftThighModel, leftHipPos);
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

    // Cuisse droite
    glm::mat4 rightThighModel = glm::mat4(1.0f);
    rightThighModel = pelvisModel;
    rightThighModel = glm::translate(rightThighModel, rightHipPos);
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

    // Dessiner les parties du corps
    drawCube(shaderProgram, torsoModel, view, projection);   // Torse
    drawCube(shaderProgram, pelvisModel, view, projection);  // Pelvis
    drawCube(shaderProgram, headModel, view, projection);    // Tête
    drawCube(shaderProgram, leftUpperArmModel, view,
             projection);  // Bras gauche (haut)
    drawCube(shaderProgram, leftForearmModel, view,
             projection);  // Avant-bras gauche
    drawCube(shaderProgram, rightUpperArmModel, view,
             projection);  // Bras droit (haut)
    drawCube(shaderProgram, rightForearmModel, view,
             projection);  // Avant-bras droit
    drawCube(shaderProgram, leftThighModel, view, projection);  // Cuisse gauche
    drawCube(shaderProgram, leftShinModel, view, projection);   // Tibia gauche
    drawCube(shaderProgram, rightThighModel, view,
             projection);                                       // Cuisse droite
    drawCube(shaderProgram, rightShinModel, view, projection);  // Tibia droit

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
