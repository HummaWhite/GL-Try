#include "EngineBase.h"

EngineBase::EngineBase(int width, int height) :
    m_Window(nullptr),
    m_ShouldTerminate(false),
    m_ErrorOccured(false), 
    m_WindowWidth(width),
    m_WindowHeight(height)
{
    this->setupGL(m_WindowWidth, m_WindowHeight);
}

EngineBase::~EngineBase()
{
}

int EngineBase::run()
{
    this->init();

    while (!this->shouldTerminate() || !glfwWindowShouldClose(this->m_Window))
    {
        this->renderLoop();
    }

    this->terminate();

    glfwTerminate();

    return this->m_ErrorOccured ? -1 : 0;
}

void EngineBase::setViewport(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);
}

void EngineBase::setupGL(int width, int height)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(width, height, "OpenGL-Try", NULL, NULL);
    if (m_Window == NULL)
    {
        this->error("Failed to create GLFW window");
        return;
    }
    glfwMakeContextCurrent(m_Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        this->error("Failed to initialize GLAD");
    }

    this->setViewport(0, 0, m_WindowWidth, m_WindowHeight);

    glfwSetWindowSizeCallback(this->m_Window, Inputs::windowSizeCallback);
    glfwSetCursorPosCallback(this->m_Window, Inputs::cursorPosCallback);
    glfwSetScrollCallback(this->m_Window, Inputs::scrollCallback);
    glfwSetKeyCallback(this->m_Window, Inputs::keyCallback);
}

void EngineBase::setTerminateStatus(bool status)
{
    m_ShouldTerminate = status;
}

void EngineBase::error(const char* errString)
{
    std::cout << "Engine Error: " << errString << std::endl;
    this->setTerminateStatus(true);
    this->m_ErrorOccured = true;
}

void EngineBase::resizeWindow(int width, int height)
{
    glfwSetWindowSize(this->m_Window, width, height);
    this->m_WindowWidth = width;
    this->m_WindowHeight = height;
}
