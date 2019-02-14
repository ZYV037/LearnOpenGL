TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LEARNOPENGL_ROOT_DIR = ../..

INCLUDEPATH +=  $$LEARNOPENGL_ROOT_DIR/glfw-3.2.1/include/ \
                $$LEARNOPENGL_ROOT_DIR/glad/include

HEADERS += \
    myshader.h

SOURCES += \
        $$LEARNOPENGL_ROOT_DIR/glad/src/glad.c  \
        main.cpp    \
        myshader.cpp

DISTFILES += \
    fragment_shader_code.frag \
    vertex_shader_code.vert


LIBS += -L$$LEARNOPENGL_ROOT_DIR/glfw-3.2.1/lib -lglfw3
