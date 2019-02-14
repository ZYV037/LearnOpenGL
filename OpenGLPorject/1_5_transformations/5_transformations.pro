TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH +=  C:\LearningOpenGL\glfw-3.2.1\include \
                C:\LearningOpenGL\glad\include

HEADERS += \
    myshader.h  \
    stb_image.h

SOURCES += \
        C:\LearningOpenGL\glad\src\glad.c \
        main.cpp    \
        myshader.cpp  \
        stb_image.cpp

DISTFILES += \
    fragment_shader_code.frag \
    vertex_shader_code.vert

LIBS += -LC:\LearningOpenGL\glfw-3.2.1\lib -lglfw3
