#ifndef SANDBOX_SHADER_OPENGL_SHADER_H_
#define SANDBOX_SHADER_OPENGL_SHADER_H_

#include "sandbox/entity.h"

#include "opengl_object.h"

#include <fstream>
#include <sstream>



class OpenGLShader : public sandbox::Component, public sandbox::ContextRenderable<OpenGLObject> {
public:
    OpenGLShader() : isLoaded(false), shaderText("") {
        addType<OpenGLShader>();
        addType<sandbox::Renderable>(static_cast<sandbox::Renderable*>(this));
        addAttribute(new sandbox::TypedAttributeRef<std::string>("shaderText", shaderText));
        addAttribute(new sandbox::TypedAttributeRef<std::string>("filePath", filePath));
        addAttribute(new sandbox::TypedAttributeRef<std::string>("shaderType", shaderType));
    }

    void update() {
        if (!isLoaded) {
            if (shaderText.length() == 0 && filePath.length() > 0) {
                std::ifstream inFile(filePath, std::ios::in);
                std::stringstream ss;
                ss << inFile.rdbuf();
                inFile.close();
                shaderText = ss.str();
            }

            if (shaderType == "vertex") {
                type = GL_VERTEX_SHADER;
            }
            else if (shaderType == "fragment") {
                type = GL_FRAGMENT_SHADER;
            }

            isLoaded = true;
        }
    }

    void updateContext(sandbox::RenderContext& context, OpenGLObject& shaderData) {
        shaderData.id = glCreateShader(type);
        const char* text = shaderText.c_str();
        glShaderSource(shaderData.id, 1, &text, NULL);
        glCompileShader(shaderData.id);
    }

    virtual void startRender(const sandbox::RenderContext& context, OpenGLObject& shaderData) {

    }

    virtual void finishRender(const sandbox::RenderContext& context, OpenGLObject& shaderData) {

    }

    sandbox::ContextObject* createContextObject() {
        std::cout << "Created shader object" << std::endl;
        return new ShaderObject();
    }

private:
    struct ShaderObject : public OpenGLObject {
        virtual ~ShaderObject() {
            glDeleteShader(id);
            std::cout << "Deleted shader " << id << std::endl;
        }
    };

    bool isLoaded;
    std::string filePath;
    std::string shaderType;
    std::string shaderText;
    GLuint type;
};

class OpenGLShaderProgram : public sandbox::Component, public sandbox::ContextRenderable<OpenGLObject> {
public:
    OpenGLShaderProgram() : isLoaded(false) {
        addType<OpenGLShaderProgram>();
        addType<sandbox::Renderable>(static_cast<sandbox::Renderable*>(this));
    }

    ~OpenGLShaderProgram() {
    }

    void update() {
        if (!isLoaded) {
            shaders = getEntity()->getComponents<OpenGLShader>();

            isLoaded = true;
        }
    }

    void updateContext(sandbox::RenderContext& context, OpenGLObject& shaderData) {
        shaderData.id = glCreateProgram();
        for (int i = 0; i < shaders.size(); i++) {
            static_cast<sandbox::Renderable*>(shaders[i])->update(context);
            glAttachShader(shaderData.id, shaders[i]->getContextObject(context).id);
        }

        glLinkProgram(shaderData.id);

        /*
        GLint status;
        glGetProgramiv(obj, GL_LINK_STATUS, &status);
        if(status == GL_FALSE) {
            GLint length;
            glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> log(length);
            glGetProgramInfoLog(obj, length, &length, &log[0]);
            std::cerr << "Error compiling program: " << &log[0] << std::endl;
            return false;
        }
        return true;
        */

        if (true) { //checkProgramLinkStatus(shaderData.id)) {
            GLint count;
            GLchar name[50];
            GLsizei length;
            GLint size;
            GLenum type;

            glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &count);

            glGetProgramiv(shaderData.id, GL_ACTIVE_ATTRIBUTES, &count);
            for (int f = 0; f < count; f++) {
                glGetActiveAttrib(shaderData.id, f, 50, &length, &size, &type, name);
                GLint loc = glGetAttribLocation(shaderData.id, name);
                std::cout << name << " " << type << " " << loc << " " << size << std::endl;
                //ShaderParameter* param = new OpenGLShaderAttribute(name, type, loc, size);
                //attributes.push_back(param);
            }

            glGetProgramiv(shaderData.id, GL_ACTIVE_UNIFORMS, &count);
            for (int f = 0; f < count; f++) {
                glGetActiveUniform(shaderData.id, f, 50, &length, &size, &type, name);


                if (size > 1) {
                    for (int i = 0; i < size; i++) {
                        std::string n(name);
                        std::stringstream ss;
                        ss << n.substr(0, n.length()-2) << i << "]";
                        n = ss.str();
                        GLint loc = glGetUniformLocation(shaderData.id, n.c_str());
                        std::cout << n << " " << type << " " << loc << " " << size << std::endl;
                        //ShaderParameter* param = createParameter(n, type, loc, size);
                        //uniforms.push_back(param);
                        //uniformMap[n] = param;
                    }
                }
                else {
                    GLint loc = glGetUniformLocation(shaderData.id, name);
                    std::cout << name << " " << type << " " << loc << " " << size << std::endl;
                        
                    //ShaderParameter* param = createParameter(name, type, loc, size);
                    //uniforms.push_back(param);
                    //uniformMap[param->getName()] = param;
                }
            }

            glGetProgramiv(shaderData.id, GL_ACTIVE_UNIFORM_BLOCKS, &count);
            for (int f = 0; f < count; f++) {
                glGetActiveUniformBlockName(shaderData.id, f, 50, &length, name);
                std::cout << name << " " << type << " " << f << " " << size << std::endl;
                //ShaderParameter* param = new UniformBufferShaderParameter(program, name, GL_UNIFORM_BUFFER, f, 1);
                //uniforms.push_back(param);
                //uniformMap[param->getName()] = param;
            }
        }
    }

    virtual void startRender(const sandbox::RenderContext& context, OpenGLObject& shaderData) {

    }

    virtual void finishRender(const sandbox::RenderContext& context, OpenGLObject& shaderData) {

    }

private:
    bool isLoaded;
    std::vector<OpenGLShader*> shaders;
};

class OpenGLShaderCommand : public sandbox::Component, public sandbox::Renderable {
public:
    OpenGLShaderCommand() : isLoaded(false), shaderProgram(NULL), prog(NULL) {
        addType<OpenGLShaderCommand>();
        addType<sandbox::Renderable>(static_cast<sandbox::Renderable*>(this));
        addAttribute(new sandbox::TypedAttributeRef<sandbox::Entity*>("shaderProgram", shaderProgram));
    }

    ~OpenGLShaderCommand() {
    }

    void update() {
        if (!isLoaded) {
            prog = shaderProgram->getComponent<OpenGLShaderProgram>();

            isLoaded = true;
        }
    }

    void updateContext(sandbox::RenderContext& context) {
        static_cast<sandbox::Renderable*>(prog)->update(context);
        context.addStack("shaderProgram", new sandbox::TypedItemStack<OpenGLShaderProgram*>());
    }

    virtual void startRender(const sandbox::RenderContext& context) {
        glUseProgram(prog->getContextObject(context).id);
        context["shaderProgram"].push<OpenGLShaderProgram*>(prog);
    }

    virtual void finishRender(const sandbox::RenderContext& context) {
        glUseProgram(0);
        context["shaderProgram"].pop();
    }

private:
    bool isLoaded;
    sandbox::Entity* shaderProgram;
    OpenGLShaderProgram* prog;
};

#endif