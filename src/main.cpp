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

// --------------------[Définition de constantes]--------------------

#define PI \
  3.14159265358979323846f  // #REF001: Définition de la constante PI utilisée
                           // pour les calculs trigonométriques.

const GLuint WIDTH = 800,
             HEIGHT =
                 600;  // #REF003: Dimensions initiales de la fenêtre OpenGL.

int windowWidth = WIDTH;
int windowHeight = HEIGHT;  // #REF005: Variables pour stocker la taille
                            // courante de la fenêtre.

GLuint VAO,
    VBO;  // #REF004: Identifiants des objets Vertex Array Object (VAO) et
          // Vertex Buffer Object (VBO) pour stocker les données des sommets.

// --------------------[Fonctions utilitaires]--------------------

// #REF002: Convertit une valeur en degrés en radians.
float degreesToRadians(float degrees) { return degrees * (PI / 180.0f); }

// #REF006: Charge le code source d'un shader à partir d'un fichier.
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

// --------------------[Initialisation de GLFW et OpenGL]--------------------

// #REF007: Initialise GLFW, une bibliothèque pour créer des fenêtres et gérer
// l'input.
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

// #REF008: Active le test de profondeur et configure la couleur de fond de la
// fenêtre.
void setupOpenGL() {
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

// --------------------[Gestion des shaders]--------------------

// #REF009: Vérifie si un shader a été compilé correctement.
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

// #REF010: Vérifie si le programme shader a été lié correctement.
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

// #REF011: Charge, compile et lie les shaders pour créer un programme shader
// OpenGL.
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

// --------------------[Gestion des buffers pour le rendu d'un
// cube]--------------------

float vertices[] = {
    // #REF012: Tableau des positions des sommets qui définissent un cube.
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

// #REF013: Prépare le VAO et VBO pour le rendu d'un cube avec les positions des
// sommets.
void setupCube() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

// --------------------[Fonctions pour le rendu du cube]--------------------

// #REF014: Dessine le cube en utilisant les matrices passées au shader.
void drawCube(GLuint shaderProgram, const Mat4& model, const Mat4& view,
              const Mat4& projection) {
  glUseProgram(shaderProgram);

  GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
  GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
  GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.m);
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.m);
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection.m);

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}

// --------------------[Callback de redimensionnement de la
// fenêtre]--------------------

void framebuffer_size_callback(
    GLFWwindow* window, int width,
    int height) {  // #REF015: Fonction callback pour ajuster la taille de la
                   // fenêtre et du viewport.
  windowWidth = width;
  windowHeight = height;
  glViewport(0, 0, width, height);
}

// --------------------[Fonctions d'animation et
// interpolation]--------------------

// #REF016: Interpolation linéaire entre deux valeurs.
float lerp(float a, float b, float t) { return a + t * (b - a); }

// #REF017: Fonction pour lisser les transitions d'animation.
float smoothstep(float edge0, float edge1, float x) {
  x = (x - edge0) / (edge1 - edge0);
  if (x < 0.0f) x = 0.0f;
  if (x > 1.0f) x = 1.0f;
  return x * x * (3 - 2 * x);
}

// --------------------[Fonction principale]--------------------

int main() {
  if (!initGLFW()) return -1;  // #REF018: Initialisation de GLFW.

  GLFWwindow* window =
      glfwCreateWindow(windowWidth, windowHeight, "HumanGL", nullptr,
                       nullptr);  // #REF019: Création de la fenêtre.
  if (!window) {
    std::cerr << "Erreur lors de la création de la fenêtre GLFW" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {  // #REF020: Initialisation de GLEW.
    std::cerr << "Erreur lors de l'initialisation de GLEW" << std::endl;
    return -1;
  }

  setupOpenGL();  // #REF021: Configuration d'OpenGL.
  setupCube();    // #REF022: Préparation du cube pour le rendu.

  GLuint shaderProgram =
      createShaderProgram();  // #REF023: Création du programme shader.
  if (shaderProgram == 0) {
    std::cerr << "Erreur lors de la création du programme shader" << std::endl;
    return -1;
  }

  // Variables pour les dimensions des parties du corps
  float headHeight = 0.8f;      // #REF024: Hauteur de la tête.
  float neckHeight = 0.2f;      // #REF025: Hauteur du cou.
  float torsoHeight = 2.2f;     // #REF026: Hauteur du torse.
  float pelvisHeight = 1.0f;    // #REF027: Hauteur du pelvis.
  float upperArmLength = 1.2f;  // #REF028: Longueur du bras supérieur.
  float forearmLength = 1.0f;   // #REF029: Longueur de l'avant-bras.
  float thighLength = 1.5f;     // #REF030: Longueur de la cuisse.
  float shinLength = 1.3f;      // #REF031: Longueur du tibia.

  // Calcul de la hauteur totale du personnage
  float totalHeight = shinLength + thighLength + pelvisHeight + torsoHeight +
                      neckHeight +
                      headHeight;  // #REF032: Hauteur totale du modèle.

  // Éloigner les bras du corps
  Vec3 leftShoulderPos(-1.05f, torsoHeight / 3.8f - 0.2f,
                       0.0f);  // #REF033: Position de l'épaule gauche.
  Vec3 rightShoulderPos(1.05f, torsoHeight / 3.8f - 0.2f,
                        0.0f);  // #REF034: Position de l'épaule droite.

  // Positions des hanches
  Vec3 leftHipPos(-0.6f, -pelvisHeight / 2.0f,
                  0.0f);  // #REF035: Position de la hanche gauche.
  Vec3 rightHipPos(0.6f, -pelvisHeight / 2.0f,
                   0.0f);  // #REF036: Position de la hanche droite.

  // Autres variables importantes pour l'animation
  float cameraDistance =
      totalHeight *
      2.5f;           // #REF037: Distance de la caméra par rapport au modèle.
  float fov = 45.0f;  // #REF038: Champ de vision de la caméra.

  // Durée totale de marche
  const float TOTAL_WALK_TIME =
      5.0f;  // #REF039: Durée totale de l'animation de marche.
  const float WALK_DURATION = 1.0f;  // #REF040: Durée d'un cycle de marche.
  const float JUMP_DURATION = 2.0f;  // #REF041: Durée du saut.
  const float JUMP_TO_IDLE_DURATION =
      2.0f;  // #REF042: Durée de la transition saut -> repos.
  const float IDLE_DURATION = 2.5f;  // #REF043: Durée de l'état de repos.

  float jumpHeight = 5.0f;  // #REF044: Hauteur maximale du saut.

  // États de l'animation
  enum AnimationState {
    WALKING,
    JUMPING,
    FROM_JUMP_TO_IDLE,
    IDLE
  };  // #REF045: État actuel de l'animation.
  AnimationState currentState =
      WALKING;  // #REF046: Initialisation de l'état courant à la marche.
  float stateTime = 0.0f;      // #REF047: Temps écoulé dans l'état actuel.
  float time = glfwGetTime();  // #REF048: Temps actuel du programme.

  // Variables de transition pour l'animation
  float prevKneeAngle = 0.0f;
  float targetKneeAngle = 0.0f;
  float prevArmSwing = 0.0f;
  float targetArmSwing = 0.0f;
  float prevVerticalMovement = 0.0f;
  float targetVerticalMovement =
      0.0f;  // #REF049: Valeurs cibles pour les mouvements de transition.

  // --------------------[Boucle principale du programme]--------------------

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT);  // #REF051: Efface le buffer couleur et
                                   // profondeur.

    float currentTime = glfwGetTime();
    float deltaTime = currentTime - time;
    time = currentTime;
    stateTime += deltaTime;  // #REF052: Met à jour le temps écoulé.

    // Variables pour les angles et mouvements des parties du corps
    float verticalMovement = 0.0f;
    float armSwing = 0.0f;
    float legSwing = 0.0f;
    float elbowAngle = 0.0f;
    float kneeAngle = 0.0f;

    // Gestion des états d'animation
    switch (currentState) {  // #REF053: Gestion des états d'animation (marche,
                             // saut, repos).
      case WALKING:
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

    // Gestion de la marche
    if (currentState == WALKING) {  // #REF054: Animation de marche avec
                                    // mouvements des bras et jambes.
      float walkCycle = fmodf(stateTime, WALK_DURATION) / WALK_DURATION;
      float walkPhase = walkCycle * 2.0f * PI;

      verticalMovement = fabsf(sinf(walkPhase)) * 0.2f;

      armSwing = sinf(walkPhase) * degreesToRadians(30.0f);
      legSwing = sinf(walkPhase) * degreesToRadians(30.0f);

      elbowAngle = degreesToRadians(15.0f) +
                   fabsf(sinf(walkPhase)) * degreesToRadians(30.0f);
      kneeAngle = fabsf(sinf(walkPhase)) * degreesToRadians(45.0f);

      // Gestion du saut
    } else if (currentState == JUMPING) {  // #REF055: Animation de saut avec
                                           // mouvement vertical.
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

      // Transition entre le saut et le repos
    } else if (currentState ==
               FROM_JUMP_TO_IDLE) {  // #REF056: Transition entre le saut et
                                     // l'état de repos.
      float t = stateTime / JUMP_TO_IDLE_DURATION;
      t = smoothstep(0.0f, 1.0f, t);
      kneeAngle = lerp(prevKneeAngle, targetKneeAngle, t);
      armSwing = lerp(prevArmSwing, targetArmSwing, t);
      verticalMovement = lerp(prevVerticalMovement, targetVerticalMovement, t);
      elbowAngle = degreesToRadians(15.0f);

      // État de repos
    } else if (currentState ==
               IDLE) {  // #REF057: Le modèle est en état de repos.
      verticalMovement = 0.0f;
      armSwing = 0.0f;
      legSwing = 0.0f;
      elbowAngle = degreesToRadians(15.0f);
      kneeAngle = 0.0f;
    }

    // Rotation du torse et inclinaison de la tête
    float torsoRotation =
        sinf(time) *
        degreesToRadians(
            5.0f);  // #REF058: Rotation du torse pendant l'animation.
    float headTilt =
        sinf(time + PI) *
        degreesToRadians(
            5.0f);  // #REF059: Inclinaison de la tête pendant l'animation.
    float selfRotationAngle =
        time * degreesToRadians(24.0f);  // #REF060: Rotation générale du modèle
                                         // pendant l'animation.

    // Transformations globales
    Mat4 globalTransform =
        Mat4::identity();  // #REF061: Matrice de transformation globale.
    globalTransform = Mat4::translate(
        globalTransform,
        Vec3(0.0f, verticalMovement + shinLength + 0.1f,
             0.0f));  // #REF062: Translation verticale du modèle.
    globalTransform = Mat4::rotate(
        globalTransform, selfRotationAngle,
        Vec3(0.0f, 1.0f, 0.0f));  // #REF063: Rotation autour de l'axe Y.
    globalTransform = Mat4::rotate(
        globalTransform, degreesToRadians(-30.0f),
        Vec3(0.0f, 1.0f,
             0.0f));  // #REF064: Rotation supplémentaire pour la vue en 3/4.

    // Matrices de vue et de projection
    Mat4 view =
        Mat4::lookAt(Vec3(0.0f, totalHeight / 2.0f, cameraDistance),
                     Vec3(0.0f, totalHeight / 2.0f, 0.0f),
                     Vec3(0.0f, 1.0f, 0.0f));  // #REF065: Vue de la caméra.

    float aspectRatio =
        static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    Mat4 projection = Mat4::perspective(
        degreesToRadians(fov), aspectRatio, 0.1f,
        1000.0f);  // #REF066: Matrice de projection en perspective.

    // Initialisation de la pile de matrices
    MatrixStack matrixStack;  // #REF067: Pile de matrices pour gérer les
                              // transformations hiérarchiques.
    matrixStack.push(globalTransform);

    // --------------------[Gestion des transformations hiérarchiques du
    // modèle]--------------------

    // Pelvis
    Mat4 pelvisMatrix = matrixStack.top();
    pelvisMatrix = Mat4::translate(
        pelvisMatrix, Vec3(0.0f, pelvisHeight / 2.0f,
                           0.0f));  // #REF068: Positionnement du pelvis.
    matrixStack.push(pelvisMatrix);
    {
      Mat4 scaledPelvis = Mat4::scale(
          pelvisMatrix,
          Vec3(1.0f, pelvisHeight, 0.8f));  // #REF069: Échelle du pelvis.
      drawCube(shaderProgram, scaledPelvis, view, projection);
    }

    // Torse
    Mat4 torsoMatrix = Mat4::translate(
        pelvisMatrix, Vec3(0.0f, pelvisHeight / 2.0f,
                           0.0f));  // #REF070: Positionnement du torse.
    torsoMatrix =
        Mat4::rotate(torsoMatrix, torsoRotation,
                     Vec3(0.0f, 1.0f, 0.0f));  // #REF071: Rotation du torse.
    matrixStack.push(torsoMatrix);
    {
      Mat4 scaledTorso = Mat4::scale(
          torsoMatrix,
          Vec3(1.2f, torsoHeight, 0.7f));  // #REF072: Échelle du torse.
      drawCube(shaderProgram, scaledTorso, view, projection);

      // Tête
      Mat4 headMatrix = Mat4::translate(
          torsoMatrix,
          Vec3(0.0f, torsoHeight / 2.0f + neckHeight + headHeight / 2.0f,
               0.0f));  // #REF073: Positionnement de la tête.
      headMatrix = Mat4::rotate(
          headMatrix, headTilt,
          Vec3(0.0f, 1.0f, 0.0f));  // #REF074: Inclinaison de la tête.
      {
        Mat4 scaledHead = Mat4::scale(
            headMatrix,
            Vec3(0.7f, headHeight, 0.8f));  // #REF075: Échelle de la tête.
        drawCube(shaderProgram, scaledHead, view, projection);
      }

      // Bras gauche
      Mat4 leftArmMatrix = Mat4::translate(
          torsoMatrix,
          leftShoulderPos);  // #REF076: Positionnement du bras gauche.
      leftArmMatrix = Mat4::rotate(
          leftArmMatrix, armSwing,
          Vec3(1.0f, 0.0f, 0.0f));  // #REF077: Mouvement du bras gauche.
      {
        Mat4 scaledLeftArm = Mat4::scale(
            leftArmMatrix, Vec3(0.3f, upperArmLength,
                                0.4f));  // #REF078: Échelle du bras gauche.
        drawCube(shaderProgram, scaledLeftArm, view, projection);

        // Avant-bras gauche
        Mat4 leftForearmMatrix = Mat4::translate(
            leftArmMatrix,
            Vec3(0.0f, -upperArmLength,
                 0.0f));  // #REF079: Positionnement de l'avant-bras gauche.
        leftForearmMatrix = Mat4::rotate(
            leftForearmMatrix, -elbowAngle,
            Vec3(1.0f, 0.0f,
                 0.0f));  // #REF080: Mouvement de l'avant-bras gauche.
        {
          Mat4 scaledLeftForearm = Mat4::scale(
              leftForearmMatrix,
              Vec3(0.25f, forearmLength,
                   0.35f));  // #REF081: Échelle de l'avant-bras gauche.
          drawCube(shaderProgram, scaledLeftForearm, view, projection);
        }
      }

      // Bras droit
      Mat4 rightArmMatrix = Mat4::translate(
          torsoMatrix,
          rightShoulderPos);  // #REF082: Positionnement du bras droit.
      rightArmMatrix = Mat4::rotate(
          rightArmMatrix, -armSwing,
          Vec3(1.0f, 0.0f, 0.0f));  // #REF083: Mouvement du bras droit.
      {
        Mat4 scaledRightArm = Mat4::scale(
            rightArmMatrix, Vec3(0.3f, upperArmLength,
                                 0.4f));  // #REF084: Échelle du bras droit.
        drawCube(shaderProgram, scaledRightArm, view, projection);

        // Avant-bras droit
        Mat4 rightForearmMatrix = Mat4::translate(
            rightArmMatrix,
            Vec3(0.0f, -upperArmLength,
                 0.0f));  // #REF085: Positionnement de l'avant-bras droit.
        rightForearmMatrix = Mat4::rotate(
            rightForearmMatrix, -elbowAngle,
            Vec3(1.0f, 0.0f,
                 0.0f));  // #REF086: Mouvement de l'avant-bras droit.
        {
          Mat4 scaledRightForearm = Mat4::scale(
              rightForearmMatrix,
              Vec3(0.25f, forearmLength,
                   0.35f));  // #REF087: Échelle de l'avant-bras droit.
          drawCube(shaderProgram, scaledRightForearm, view, projection);
        }
      }
    }
    matrixStack.pop();  // Fin du torse

    // Jambe gauche
    Mat4 leftLegMatrix = Mat4::translate(
        pelvisMatrix,
        leftHipPos);  // #REF088: Positionnement de la jambe gauche.
    leftLegMatrix = Mat4::rotate(
        leftLegMatrix, legSwing,
        Vec3(1.0f, 0.0f, 0.0f));  // #REF089: Mouvement de la jambe gauche.
    {
      Mat4 scaledLeftLeg = Mat4::scale(
          leftLegMatrix, Vec3(0.4f, thighLength,
                              0.6f));  // #REF090: Échelle de la cuisse gauche.
      drawCube(shaderProgram, scaledLeftLeg, view, projection);

      // Mollet gauche
      Mat4 leftShinMatrix = Mat4::translate(
          leftLegMatrix,
          Vec3(0.0f, -thighLength,
               0.0f));  // #REF091: Positionnement du mollet gauche.
      leftShinMatrix = Mat4::rotate(
          leftShinMatrix, kneeAngle,
          Vec3(1.0f, 0.0f, 0.0f));  // #REF092: Mouvement du genou gauche.
      {
        Mat4 scaledLeftShin = Mat4::scale(
            leftShinMatrix, Vec3(0.4f, shinLength,
                                 0.5f));  // #REF093: Échelle du mollet gauche.
        drawCube(shaderProgram, scaledLeftShin, view, projection);
      }
    }

    // Jambe droite
    Mat4 rightLegMatrix = Mat4::translate(
        pelvisMatrix,
        rightHipPos);  // #REF094: Positionnement de la jambe droite.
    rightLegMatrix = Mat4::rotate(
        rightLegMatrix, -legSwing,
        Vec3(1.0f, 0.0f, 0.0f));  // #REF095: Mouvement de la jambe droite.
    {
      Mat4 scaledRightLeg = Mat4::scale(
          rightLegMatrix, Vec3(0.4f, thighLength,
                               0.6f));  // #REF096: Échelle de la cuisse droite.
      drawCube(shaderProgram, scaledRightLeg, view, projection);

      // Mollet droit
      Mat4 rightShinMatrix = Mat4::translate(
          rightLegMatrix,
          Vec3(0.0f, -thighLength,
               0.0f));  // #REF097: Positionnement du mollet droit.
      rightShinMatrix = Mat4::rotate(
          rightShinMatrix, kneeAngle,
          Vec3(1.0f, 0.0f, 0.0f));  // #REF098: Mouvement du genou droit.
      {
        Mat4 scaledRightShin = Mat4::scale(
            rightShinMatrix,
            Vec3(0.4f, shinLength, 0.5f));  // #REF099: Échelle du mollet droit.
        drawCube(shaderProgram, scaledRightShin, view, projection);
      }
    }
    matrixStack.pop();  // Fin du pelvis
    matrixStack.pop();  // Fin de la pile

    glfwSwapBuffers(window);  // #REF100: Échange les buffers de la fenêtre pour
                              // afficher le rendu.
    glfwPollEvents();  // #REF101: Traite les événements de la fenêtre.
  }

  glfwDestroyWindow(window);  // #REF102: Détruit la fenêtre.
  glfwTerminate();            // #REF103: Termine l'utilisation de GLFW.

  return 0;
}
