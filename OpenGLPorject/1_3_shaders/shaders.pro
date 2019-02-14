TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH +=  C:\LearningOpenGL\glfw-3.2.1\include \
                C:\LearningOpenGL\glad\include


HEADERS += \
    myshader.h

SOURCES += \
        main.cpp    \
        C:\LearningOpenGL\glad\src\glad.c \
        myshader.cpp

LIBS += -LC:\LearningOpenGL\glfw-3.2.1\lib -lglfw3

DISTFILES += \
    fragment_shader_code.frag \
    vertex_shader_code.vert
