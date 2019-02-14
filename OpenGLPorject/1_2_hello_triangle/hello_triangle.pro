TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LEARNOPENGL_ROOT_DIR = ../..

INCLUDEPATH +=  $$LEARNOPENGL_ROOT_DIR/glfw-3.2.1/include \
                $$LEARNOPENGL_ROOT_DIR/glad/include

SOURCES += \
        main.cpp    \
        $$LEARNOPENGL_ROOT_DIR/glad/src/glad.c

LIBS += -L$$LEARNOPENGL_ROOT_DIR/glfw-3.2.1/lib -lglfw3
