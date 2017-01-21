/* Lab 6 base code - transforms using matrix stack built on glm
        CPE 471 Cal Poly Z. Wood + S. Sueda
*/
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

/* to use glee */
#define GLEE_OVERWRITE_GL_FUNCTIONS
#include "glee.hpp"

GLFWwindow* window;        // Main application window
string RESOURCE_DIR = "";  // Where the resources are loaded from
shared_ptr<Program> program;
shared_ptr<Shape> shape;

int g_width, g_height;

static void error_callback(int error, const char* description) {
  cerr << description << endl;
}

static void key_callback(GLFWwindow* window,
                         int key,
                         int scancode,
                         int action,
                         int mods) {
  std::cout << __FUNCTION__ << " key: " << key << " action: " << action << std::endl;
  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}

static void mouse_callback(GLFWwindow* window,
                           int button,
                           int action,
                           int mods) {
  double posX, posY;
  if (action == GLFW_PRESS) {
    glfwGetCursorPos(window, &posX, &posY);
    cout << "Pos X " << posX << " Pos Y " << posY << endl;
  }
}

static void resize_callback(GLFWwindow* window, int width, int height) {
  g_width = width;
  g_height = height;
  glViewport(0, 0, width, height);
}

static void init() {
  GLSL::checkVersion();

  // Set background color.
  glClearColor(.12f, .34f, .56f, 1.0f);
  // Enable z-buffer test.
  glEnable(GL_DEPTH_TEST);

  // Initialize mesh.
  shape = make_shared<Shape>();
  shape->loadMesh(RESOURCE_DIR + "bunny.obj");
  shape->resize();
  shape->init();

  program = make_shared<Program>();
  program->setVerbose(true);
  program->setShaderNames(RESOURCE_DIR + "simple_vert.glsl", RESOURCE_DIR + "simple_frag.glsl");
  program->init();
  program->addUniform("P");
  program->addUniform("V");
  program->addUniform("MV");
  program->addAttribute("vertPos");
  program->addAttribute("vertNor");
}

static void render() {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  float aspect = width / (float)height;

  program->bind();
  auto P = make_shared<MatrixStack>();
  auto V = make_shared<MatrixStack>();
  auto MV = make_shared<MatrixStack>();
  P->pushMatrix();
  P->perspective(45.0f, aspect, 0.01f, 100.0f);
  glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE,
                     value_ptr(P->topMatrix()));

  V->pushMatrix();
  V->loadIdentity();
  V->lookAt(vec3(0, 0, 0), vec3(0, 0, 1), vec3(0, 1, 0)); // (eye, target, up)
  glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE,
                     value_ptr(V->topMatrix()));
  V->popMatrix();

  MV->pushMatrix();
  MV->loadIdentity();
  MV->translate(vec3(0, 0, 5));
  static unsigned frame = 0;
  frame++;
  float time = (frame % 200) / 200.0f; // 0 to 1
  float rotation = time * 2.0f * 3.14159f;
  MV->rotate(rotation, vec3(0, 1, 0));
  glUniformMatrix4fv(program->getUniform("MV"), 1, GL_FALSE,
                     value_ptr(MV->topMatrix()));
  shape->draw(program);
  MV->popMatrix();

  P->popMatrix();
}

int main(int argc, char** argv) {
  if (argc < 2) {
    cout << "Please specify the resource directory." << endl;
    return 0;
  }
  RESOURCE_DIR = argv[1] + string("/");

  // Set error callback.
  glfwSetErrorCallback(error_callback);
  // Initialize the library.
  if (!glfwInit()) {
    return -1;
  }
  // request the highest possible version of OGL - important for mac
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  // Create a windowed mode window and its OpenGL context.
  window = glfwCreateWindow(640, 480, "Joseph Arhar", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  // Make the window's context current.
  glfwMakeContextCurrent(window);
  // Initialize GLEW.
  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    cerr << "Failed to initialize GLEW" << endl;
    return -1;
  }
  // weird bootstrap of glGetError
  glGetError();
  cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
  cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

  // Set vsync.
  glfwSwapInterval(1);
  // Set keyboard callback.
  glfwSetKeyCallback(window, key_callback);
  // set the mouse call back
  glfwSetMouseButtonCallback(window, mouse_callback);
  // set the window resize call back
  glfwSetFramebufferSizeCallback(window, resize_callback);

  // Initialize scene. Note geometry initialized in init now
  init();

  // Loop until the user closes the window.
  while (!glfwWindowShouldClose(window)) {
    // Render scene.
    render();
    // Swap front and back buffers.
    glfwSwapBuffers(window);
    // Poll for and process events.
    glfwPollEvents();
  }
  // Quit program.
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
