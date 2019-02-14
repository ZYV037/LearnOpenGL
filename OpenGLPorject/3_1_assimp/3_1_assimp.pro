TEMPLATE = app
CONFIG += c++11 console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH +=  C:\LearningOpenGL\glfw-3.2.1\include \
                C:\LearningOpenGL\glad\include  \
                C:\LearningOpenGL   \
                C:\LearningOpenGL\assimp    \

HEADERS += \
    myshader.h  \
    stb_image.h \
    mesh.h \
    model.h \
    camera.h

SOURCES += \
    C:\LearningOpenGL\glad\src\glad.c \
    main.cpp    \
    myshader.cpp  \
    stb_image.cpp   \
    mesh.cpp \
    model.cpp \
    camera.cpp

DISTFILES += \
    fragment_shader_code.frag \
    vertex_shader_code.vert

LIBS += -LC:\LearningOpenGL\glfw-3.2.1\lib -lglfw3  \
        -LC:\LearningOpenGL\assimp\lib -lassimp
