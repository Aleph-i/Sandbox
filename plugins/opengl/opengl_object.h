#ifndef SANDBOX_OPENGL_OBJECT_H_
#define SANDBOX_OPENGL_OBJECT_H_

#include "sandbox_graphics/renderable.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>


struct OpenGLObject : public sandbox::ContextObject {
    OpenGLObject() : id(GL_NONE) {}
    virtual ~OpenGLObject() {}
    GLuint id;
};


#endif