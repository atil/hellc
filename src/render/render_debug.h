#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <iostream>

inline void check_gl_error(const std::string& tag) {
    const int error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "GL error [" << tag << "] error code: [" << error << "]" << std::endl;
    }
}

inline void check_gl_framebuffer_complete(const std::string& tag) {
    const int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << " Framebuffer isn't complete: " << tag << std::endl;
    }
}
