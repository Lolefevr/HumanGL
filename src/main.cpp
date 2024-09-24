// main.cpp

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// Inclure GLEW
#include <GL/glew.h>

// Inclure GLFW
#include <GLFW/glfw3.h>

// Inclure votre propre bibliothèque mathématique
#include "my_math.h"

// Définition de PI
#define PI 3.14159265358979323846f

// Fonction pour convertir les degrés en radians
float degreesToRadians(float degrees) { return degrees * (PI / 180.0f); }

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
  glEnable(GL_DEPTH_TEST);
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
  std::string vertexCode = loadShaderSource("include/vertex_shader.glsl");
  std::string fragmentCode = loadShaderSource("include/fragment_shader.glsl");

  if (vertexCode.empty() || fragmentCode.empty()) {
    std::cerr << "Erreur : le code des shaders est vide." << std::endl;
    return 0;
  }

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  const char* vertexSource = vertexCode.c_str();
  glShaderSource(vertexShader, 1, &vertexSource, nullptr);
  glCompileShader(vertexShader);
  checkShaderCompilation(vertexShader, "vertex");

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  const char* fragmentSource = fragmentCode.c_str();
  glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
  glCompileShader(fragmentShader);
  checkShaderCompilation(fragmentShader, "fragment");

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  checkProgramLinking(shaderProgram);

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
    0.5f, 0.5f, 0.5f,     // Haut avant
    0.5f, 0.5f, -0.5f,    // Haut arrière
    0.5f, -0.5f, -0.5f,   // Bas arrière
    0.5f, -0.5f, -0.5f,   // Bas arrière
    0.5f, -0.5f, 0.5f,    // Bas avant
    0.5f, 0.5f, 0.5f,     // Haut avant
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
void drawCube(GLuint shaderProgram, const Mat4& model, const Mat4& view,
              const Mat4& projection) {
  glUseProgram(shaderProgram);

  GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
  GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
  GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

  // Passer les matrices au shader
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.m);
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.m);
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection.m);

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
  x = (x - edge0) / (edge1 - edge0);
  if (x < 0.0f) x = 0.0f;
  if (x > 1.0f) x = 1.0f;
  return x * x * (3 - 2 * x);
}

int main() {
  if (!initGLFW()) return -1;

  GLFWwindow* window =
      glfwCreateWindow(windowWidth, windowHeight, "HumanGL", nullptr, nullptr);
  if (!window) {
    std::cerr << "Erreur lors de la création de la fenêtre GLFW" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    std::cerr << "Erreur lors de l'initialisation de GLEW" << std::endl;
    return -1;
  }

  setupOpenGL();
  setupCube();

  GLuint shaderProgram = createShaderProgram();
  if (shaderProgram == 0) {
    std::cerr << "Erreur lors de la création du programme shader" << std::endl;
    return -1;
  }

  float headHeight = 0.8f;
  float neckHeight = 0.2f;
  float torsoHeight = 2.2f;
  float pelvisHeight = 1.0f;
  float upperArmLength = 1.2f;  // Augmenté pour des bras plus longs
  float forearmLength = 1.0f;   // Augmenté pour des avant-bras plus longs
  float thighLength = 1.5f;
  float shinLength = 1.3f;

  float totalHeight = shinLength + thighLength + pelvisHeight + torsoHeight +
                      neckHeight + headHeight;

  // Éloigner les bras du corps en augmentant la valeur absolue de x
  Vec3 leftShoulderPos(-1.05f, torsoHeight / 3.8f - 0.2f, 0.0f);
  Vec3 rightShoulderPos(1.05f, torsoHeight / 3.8f - 0.2f, 0.0f);

  Vec3 leftHipPos(-0.6f, -pelvisHeight / 2.0f, 0.0f);
  Vec3 rightHipPos(0.6f, -pelvisHeight / 2.0f, 0.0f);

  float cameraDistance = totalHeight * 2.5f;
  float fov = 45.0f;

  // Nouvelle variable pour la durée totale de marche
  const float TOTAL_WALK_TIME = 5.0f;  // Exemple : marche pendant 5 secondes
  const float WALK_DURATION = 1.0f;
  const float JUMP_DURATION = 2.0f;
  const float JUMP_TO_IDLE_DURATION = 2.0f;
  const float IDLE_DURATION = 2.5f;

  float jumpHeight = 5.0f;

  enum AnimationState { WALKING, JUMPING, FROM_JUMP_TO_IDLE, IDLE };
  AnimationState currentState = WALKING;
  float stateTime = 0.0f;
  float time = glfwGetTime();

  float prevKneeAngle = 0.0f;
  float targetKneeAngle = 0.0f;
  float prevArmSwing = 0.0f;
  float targetArmSwing = 0.0f;
  float prevVerticalMovement = 0.0f;
  float targetVerticalMovement = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float currentTime = glfwGetTime();
    float deltaTime = currentTime - time;
    time = currentTime;
    stateTime += deltaTime;

    float verticalMovement = 0.0f;
    float armSwing = 0.0f;
    float legSwing = 0.0f;
    float elbowAngle = 0.0f;
    float kneeAngle = 0.0f;

    switch (currentState) {
      case WALKING:
        // Utilisation de TOTAL_WALK_TIME pour déterminer quand arrêter la
        // marche
        if (stateTime >= TOTAL_WALK_TIME) {
          currentState = JUMPING;
          stateTime = 0.0f;
        }
        break;
      case JUMPING:
        if (stateTime >= JUMP_DURATION) {
          prevKneeAngle = kneeAngle;
          prevArmSwing = armSwing;
          prevVerticalMovement = verticalMovement;

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

    if (currentState == WALKING) {
      float walkCycle = fmodf(stateTime, WALK_DURATION) / WALK_DURATION;
      float walkPhase = walkCycle * 2.0f * PI;

      verticalMovement = fabsf(sinf(walkPhase)) * 0.2f;

      armSwing = sinf(walkPhase) * degreesToRadians(30.0f);
      legSwing = sinf(walkPhase) * degreesToRadians(30.0f);

      elbowAngle = degreesToRadians(15.0f) +
                   fabsf(sinf(walkPhase)) * degreesToRadians(30.0f);
      kneeAngle = fabsf(sinf(walkPhase)) * degreesToRadians(45.0f);

    } else if (currentState == JUMPING) {
      float jumpProgress = stateTime / JUMP_DURATION;

      if (jumpProgress < 0.2f) {
        float crouchAmount = sinf(jumpProgress / 0.2f * (PI / 2.0f));
        verticalMovement = -crouchAmount * 0.5f;
        kneeAngle = crouchAmount * degreesToRadians(60.0f);
        armSwing = -crouchAmount * degreesToRadians(30.0f);
      } else if (jumpProgress < 0.8f) {
        float jumpPhase = (jumpProgress - 0.2f) / 0.6f * PI;
        verticalMovement = sinf(jumpPhase) * jumpHeight;
        kneeAngle = (1.0f - sinf(jumpPhase)) * degreesToRadians(30.0f);
        armSwing = sinf(jumpPhase) * degreesToRadians(20.0f);
      } else {
        float landProgress = (jumpProgress - 0.8f) / 0.2f;
        float crouchAmount = sinf(landProgress * (PI / 2.0f));
        verticalMovement = -crouchAmount * 0.5f;
        kneeAngle = crouchAmount * degreesToRadians(60.0f);
        armSwing = -crouchAmount * degreesToRadians(30.0f);
      }

      elbowAngle = degreesToRadians(10.0f);

    } else if (currentState == FROM_JUMP_TO_IDLE) {
      float t = stateTime / JUMP_TO_IDLE_DURATION;
      t = smoothstep(0.0f, 1.0f, t);
      kneeAngle = lerp(prevKneeAngle, targetKneeAngle, t);
      armSwing = lerp(prevArmSwing, targetArmSwing, t);
      verticalMovement = lerp(prevVerticalMovement, targetVerticalMovement, t);
      elbowAngle = degreesToRadians(15.0f);
    } else if (currentState == IDLE) {
      verticalMovement = 0.0f;
      armSwing = 0.0f;
      legSwing = 0.0f;
      elbowAngle = degreesToRadians(15.0f);
      kneeAngle = 0.0f;
    }

    float torsoRotation = sinf(time) * degreesToRadians(5.0f);
    float headTilt = sinf(time + PI) * degreesToRadians(5.0f);
    float selfRotationAngle = time * degreesToRadians(24.0f);

    Mat4 globalTransform = Mat4::identity();
    globalTransform =
        Mat4::translate(globalTransform,
                        Vec3(0.0f, verticalMovement + shinLength + 0.1f, 0.0f));
    globalTransform = Mat4::rotate(globalTransform, selfRotationAngle,
                                   Vec3(0.0f, 1.0f, 0.0f));
    globalTransform = Mat4::rotate(globalTransform, degreesToRadians(-30.0f),
                                   Vec3(0.0f, 1.0f, 0.0f));

    Mat4 view = Mat4::lookAt(Vec3(0.0f, totalHeight / 2.0f, cameraDistance),
                             Vec3(0.0f, totalHeight / 2.0f, 0.0f),
                             Vec3(0.0f, 1.0f, 0.0f));

    float aspectRatio =
        static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    Mat4 projection =
        Mat4::perspective(degreesToRadians(fov), aspectRatio, 0.1f, 1000.0f);

    MatrixStack matrixStack;
    matrixStack.push(globalTransform);

    // Pelvis
    Mat4 pelvisMatrix = matrixStack.top();
    pelvisMatrix =
        Mat4::translate(pelvisMatrix, Vec3(0.0f, pelvisHeight / 2.0f, 0.0f));
    matrixStack.push(pelvisMatrix);
    {
      Mat4 scaledPelvis =
          Mat4::scale(pelvisMatrix, Vec3(1.0f, pelvisHeight, 0.8f));
      drawCube(shaderProgram, scaledPelvis, view, projection);
    }

    // Torse
    Mat4 torsoMatrix =
        Mat4::translate(pelvisMatrix, Vec3(0.0f, pelvisHeight / 2.0f, 0.0f));
    torsoMatrix =
        Mat4::rotate(torsoMatrix, torsoRotation, Vec3(0.0f, 1.0f, 0.0f));
    matrixStack.push(torsoMatrix);
    {
      Mat4 scaledTorso =
          Mat4::scale(torsoMatrix, Vec3(1.2f, torsoHeight, 0.7f));
      drawCube(shaderProgram, scaledTorso, view, projection);

      // Tête
      Mat4 headMatrix = Mat4::translate(
          torsoMatrix,
          Vec3(0.0f, torsoHeight / 2.0f + neckHeight + headHeight / 2.0f,
               0.0f));
      headMatrix = Mat4::rotate(headMatrix, headTilt, Vec3(0.0f, 1.0f, 0.0f));
      {
        Mat4 scaledHead = Mat4::scale(headMatrix, Vec3(0.7f, headHeight, 0.8f));
        drawCube(shaderProgram, scaledHead, view, projection);
      }

      // Bras gauche
      Mat4 leftArmMatrix = Mat4::translate(torsoMatrix, leftShoulderPos);
      leftArmMatrix =
          Mat4::rotate(leftArmMatrix, armSwing, Vec3(1.0f, 0.0f, 0.0f));
      {
        Mat4 scaledLeftArm =
            Mat4::scale(leftArmMatrix, Vec3(0.3f, upperArmLength, 0.4f));
        drawCube(shaderProgram, scaledLeftArm, view, projection);

        // Avant-bras gauche
        Mat4 leftForearmMatrix =
            Mat4::translate(leftArmMatrix, Vec3(0.0f, -upperArmLength, 0.0f));
        leftForearmMatrix =
            Mat4::rotate(leftForearmMatrix, -elbowAngle,
                         Vec3(1.0f, 0.0f, 0.0f));  // Ajustement ici
        {
          Mat4 scaledLeftForearm =
              Mat4::scale(leftForearmMatrix, Vec3(0.25f, forearmLength, 0.35f));
          drawCube(shaderProgram, scaledLeftForearm, view, projection);
        }
      }

      // Bras droit
      Mat4 rightArmMatrix = Mat4::translate(torsoMatrix, rightShoulderPos);
      rightArmMatrix =
          Mat4::rotate(rightArmMatrix, -armSwing, Vec3(1.0f, 0.0f, 0.0f));
      {
        Mat4 scaledRightArm =
            Mat4::scale(rightArmMatrix, Vec3(0.3f, upperArmLength, 0.4f));
        drawCube(shaderProgram, scaledRightArm, view, projection);

        // Avant-bras droit
        Mat4 rightForearmMatrix =
            Mat4::translate(rightArmMatrix, Vec3(0.0f, -upperArmLength, 0.0f));
        rightForearmMatrix =
            Mat4::rotate(rightForearmMatrix, -elbowAngle,
                         Vec3(1.0f, 0.0f, 0.0f));  // Ajustement ici
        {
          Mat4 scaledRightForearm = Mat4::scale(
              rightForearmMatrix, Vec3(0.25f, forearmLength, 0.35f));
          drawCube(shaderProgram, scaledRightForearm, view, projection);
        }
      }
    }
    matrixStack.pop();  // Fin du torse

    // Jambe gauche
    Mat4 leftLegMatrix = Mat4::translate(pelvisMatrix, leftHipPos);
    leftLegMatrix =
        Mat4::rotate(leftLegMatrix, legSwing, Vec3(1.0f, 0.0f, 0.0f));
    {
      Mat4 scaledLeftLeg =
          Mat4::scale(leftLegMatrix, Vec3(0.4f, thighLength, 0.6f));
      drawCube(shaderProgram, scaledLeftLeg, view, projection);

      // Mollet gauche
      Mat4 leftShinMatrix =
          Mat4::translate(leftLegMatrix, Vec3(0.0f, -thighLength, 0.0f));
      leftShinMatrix = Mat4::rotate(leftShinMatrix, kneeAngle,
                                    Vec3(1.0f, 0.0f, 0.0f));  // Genoux corrects
      {
        Mat4 scaledLeftShin =
            Mat4::scale(leftShinMatrix, Vec3(0.4f, shinLength, 0.5f));
        drawCube(shaderProgram, scaledLeftShin, view, projection);
      }
    }

    // Jambe droite
    Mat4 rightLegMatrix = Mat4::translate(pelvisMatrix, rightHipPos);
    rightLegMatrix =
        Mat4::rotate(rightLegMatrix, -legSwing, Vec3(1.0f, 0.0f, 0.0f));
    {
      Mat4 scaledRightLeg =
          Mat4::scale(rightLegMatrix, Vec3(0.4f, thighLength, 0.6f));
      drawCube(shaderProgram, scaledRightLeg, view, projection);

      // Mollet droit
      Mat4 rightShinMatrix =
          Mat4::translate(rightLegMatrix, Vec3(0.0f, -thighLength, 0.0f));
      rightShinMatrix =
          Mat4::rotate(rightShinMatrix, kneeAngle,
                       Vec3(1.0f, 0.0f, 0.0f));  // Genoux corrects
      {
        Mat4 scaledRightShin =
            Mat4::scale(rightShinMatrix, Vec3(0.4f, shinLength, 0.5f));
        drawCube(shaderProgram, scaledRightShin, view, projection);
      }
    }
    matrixStack.pop();  // Fin du pelvis
    matrixStack.pop();  // Fin de la pile

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
