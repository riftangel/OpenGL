// needed for printf(), and rand(), they are not required
#include <cstdio>
#include <cstdlib>
#include <ctime>

// you will need your own OpenGL bindings, GLFW (or other lib capable of
// creating window and OpenGL context) and SOIL (or, again, other lib loading
// images in format of your choice)
#include "libs.h"

// not necessary but makes my life much easier when I can just see the number
// of bits and I dont have to write 'unsigned' as a separate word
typedef short              int16;
typedef int                int32;
typedef unsigned short     uint16;
typedef unsigned int       uint32;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
GLuint getShaderProgramId(const char* vertexFile, const char* fragmentFile);
GLuint compileShader(const GLchar* source, GLuint shaderType);

GLFWwindow* window;

const uint32
WINDOW_WIDTH = 1920,
WINDOW_HEIGHT = 1080,
// object count is static number in this example, but it can be made a bit
// more flexible (however remember that you will never have much freedom
// with constant block of memory which is required by targeted performance)
OBJECT_COUNT = 100;

GLuint shaderProgramId, vao, vbo, ubo, textureId;

// in order to avoid sending information to shaders screen resolution is hard
// coded (precisely, half of it in each dimension), but it should be
// unnoticable in performance after extracting to shader uniform
const char* vertexShader =
"#version 330\n"
"layout (location = 0) in vec2 vert;\n"
"layout (location = 1) in vec2 _uv;\n"
"out vec2 uv;\n"
"void main()\n"
"{\n"
"    uv = _uv;\n"
"    gl_Position = vec4(vert.x / 720.0 - 1.0, vert.y / 405.0 - 1.0, 0.0, 1.0);\n"
"}\n";

const char* fragmentShader =
"#version 330\n"
"out vec4 color;\n"
"in vec2 uv;\n"
"uniform sampler2D tex;\n"
"void main()\n"
"{\n"
"    color = texture(tex, uv);\n"
"}\n";

struct Texture { uint16 width, height; float u1, v1, u2, v2; };
struct Object { int16 x, y; Texture texture; };

Texture
watermelon = { 64, 64, 0.0f,   0.0f,   0.5f,   0.5f },
pineapple = { 64, 64, 0.5f,   0.0f,   1.0f,   0.5f },
orange = { 32, 32, 0.0f,   0.5f,   0.25f,  0.75f },
grapes = { 32, 32, 0.25f,  0.5f,   0.5f,   0.75f },
pear = { 32, 32, 0.0f,   0.75f,  0.25f,  1.0f },
banana = { 32, 32, 0.25f,  0.75f,  0.5f,   1.0f },
strawberry = { 16, 16, 0.5f,   0.5f,   0.625f, 0.625f },
raspberry = { 16, 16, 0.625f, 0.5f,   0.75f,  0.625f },
cherries = { 16, 16, 0.5f,   0.625f, 0.625f, 0.75f };

Texture textures[9] =
{
    watermelon,
    pineapple,
    orange,
    grapes,
    pear,
    banana,
    strawberry,
    cherries,
    raspberry
};

static Object objects[OBJECT_COUNT];

static int16 vertices[OBJECT_COUNT * 12];
static float uvs[OBJECT_COUNT * 12];

void updateObject(int i)
{
    // top right
    vertices[i * 12] = objects[i].x + objects[i].texture.width;
    vertices[i * 12 + 1] = objects[i].y;

    // bottom right
    vertices[i * 12 + 2] = objects[i].x + objects[i].texture.width;
    vertices[i * 12 + 3] = objects[i].y + objects[i].texture.height;

    // top left
    vertices[i * 12 + 4] = objects[i].x;
    vertices[i * 12 + 5] = objects[i].y;

    // bottom right
    vertices[i * 12 + 6] = objects[i].x + objects[i].texture.width;
    vertices[i * 12 + 7] = objects[i].y + objects[i].texture.height;

    // bottom left
    vertices[i * 12 + 8] = objects[i].x;
    vertices[i * 12 + 9] = objects[i].y + objects[i].texture.height;

    // top left
    vertices[i * 12 + 10] = objects[i].x;
    vertices[i * 12 + 11] = objects[i].y;
}

void update()
{
    // dancing fruits =)
    for (uint32 i = 0; i < OBJECT_COUNT; i++)
    {
        objects[i].x += rand() % 5 - 2;
        objects[i].y += rand() % 5 - 2;

        updateObject(i);
    }

    // if you had to unbind vbo for whatever reason, bind it again now
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

}

void render()
{
    glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // if you had to unbind vao for whatever reason, bind it again now
    // glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, OBJECT_COUNT * 6);

    glfwSwapBuffers(window);
}

int32 main_test()
{
    // not perfect but good enough randomness
    srand(time(0));

    // initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "", 0, 0);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    // initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    // OpenGL setup
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);

    // viewport setup
    {
        int32 width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
    }

    // initialize shader
    shaderProgramId = getShaderProgramId(vertexShader, fragmentShader);

    // texture
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // load image using SOIL and store it on GPU
    {
        int32 width, height;
        unsigned char* image = SOIL_load_image(
            "texture.png",
            &width,
            &height,
            0,
            SOIL_LOAD_RGBA);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            image);

        if (image == 0)
        {
            printf("Failed to load texture image.\n");
            exit(1);
        }
        else
        {
            SOIL_free_image_data(image);
        }
    }

    for (uint32 i = 0; i < OBJECT_COUNT; i++)
    {
        Texture t = textures[rand() % 9];
        objects[i] =
        {
            (int16)(rand() % (WINDOW_WIDTH - t.width)),
            (int16)(rand() % (WINDOW_HEIGHT - t.height)),
            t
        };

        // vertices
        {
            // top right
            vertices[i * 12] = objects[i].x + objects[i].texture.width;
            vertices[i * 12 + 1] = objects[i].y;

            // bottom right
            vertices[i * 12 + 2] = objects[i].x + objects[i].texture.width;
            vertices[i * 12 + 3] = objects[i].y + objects[i].texture.height;

            // top left
            vertices[i * 12 + 4] = objects[i].x;
            vertices[i * 12 + 5] = objects[i].y;

            // bottom right
            vertices[i * 12 + 6] = objects[i].x + objects[i].texture.width;
            vertices[i * 12 + 7] = objects[i].y + objects[i].texture.height;

            // bottom left
            vertices[i * 12 + 8] = objects[i].x;
            vertices[i * 12 + 9] = objects[i].y + objects[i].texture.height;

            // top left
            vertices[i * 12 + 10] = objects[i].x;
            vertices[i * 12 + 11] = objects[i].y;
        }

        // uvs
        {
            // top right
            uvs[i * 12] = objects[i].texture.u2;
            uvs[i * 12 + 1] = objects[i].texture.v2;

            // bottom right
            uvs[i * 12 + 2] = objects[i].texture.u2;
            uvs[i * 12 + 3] = objects[i].texture.v1;

            // top left
            uvs[i * 12 + 4] = objects[i].texture.u1;
            uvs[i * 12 + 5] = objects[i].texture.v2;

            // bottom right
            uvs[i * 12 + 6] = objects[i].texture.u2;
            uvs[i * 12 + 7] = objects[i].texture.v1;

            // bottom left
            uvs[i * 12 + 8] = objects[i].texture.u1;
            uvs[i * 12 + 9] = objects[i].texture.v1;

            // top left
            uvs[i * 12 + 10] = objects[i].texture.u1;
            uvs[i * 12 + 11] = objects[i].texture.v2;
        }
    }

    // initialize OpenGL buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ubo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_SHORT, GL_FALSE, 2 * sizeof(int16), 0);

    glBindBuffer(GL_ARRAY_BUFFER, ubo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 2 * sizeof(GLfloat), 0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // those usually go to render(), but this example uses just one shader, vao
    // and texture so it is enough to set them once
    glUseProgram(shaderProgramId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glBindVertexArray(vao);

    // main loop
    double t = 0.0;
    const double dt = 0.01;

    double currentTime = glfwGetTime();
    double lastPrinted = currentTime;
    double accumulator = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        double newTime = glfwGetTime();
        double frameTime = newTime - currentTime;
        currentTime = newTime;

        // print frame time (if you want to get rid of that remember to also
        // delete lastPrinted variable above
        if (currentTime - lastPrinted > 1.0)
        {
            printf("%fms\n", frameTime * 1000.0);
            lastPrinted = currentTime;
        }

        accumulator += frameTime;

        while (accumulator >= dt)
        {
            glfwPollEvents();
            update();
            accumulator -= dt;
            t += dt;
        }

        render();
    }

    // cleanup
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ubo);
    glDeleteTextures(1, &textureId);
    glDeleteProgram(shaderProgramId);

    glfwTerminate();
    return 0;
}

void key_callback(
    GLFWwindow* window,
    int key,
    int32 scancode,
    int32 action,
    int32 mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

GLuint getShaderProgramId(const char* vertexFile, const char* fragmentFile)
{
    GLuint programId, vertexHandler, fragmentHandler;

    vertexHandler = compileShader(vertexFile, GL_VERTEX_SHADER);
    fragmentHandler = compileShader(fragmentFile, GL_FRAGMENT_SHADER);

    programId = glCreateProgram();
    glAttachShader(programId, vertexHandler);
    glAttachShader(programId, fragmentHandler);
    glLinkProgram(programId);

    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, 512, 0, infoLog);
        printf("Error in linking of shaders:\n%s\n", infoLog);
        exit(1);
    }

    glDeleteShader(vertexHandler);
    glDeleteShader(fragmentHandler);

    return programId;
}

GLuint compileShader(const GLchar* source, GLuint shaderType)
{
    GLuint shaderHandler;

    shaderHandler = glCreateShader(shaderType);
    glShaderSource(shaderHandler, 1, &source, 0);
    glCompileShader(shaderHandler);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shaderHandler, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderHandler, 512, 0, infoLog);
        printf("Error in compilation of shader:\n%s\n", infoLog);
        exit(1);
    };

    return shaderHandler;
}
