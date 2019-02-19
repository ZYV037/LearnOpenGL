TEMPLATE = app
CONFIG += c++14 console
CONFIG -= app_bundle
CONFIG -= qt

LEARNOPENGL_ROOT_DIR = ../..

INCLUDEPATH +=  $$LEARNOPENGL_ROOT_DIR/glfw-3.2.1/include/ \
                $$LEARNOPENGL_ROOT_DIR/glad/include \
                $$LEARNOPENGL_ROOT_DIR/

HEADERS += \
    $$LEARNOPENGL_ROOT_DIR/stb_image/stb_image.h    \
    myshader.h  \
    mesh.h \
    model.h \
    camera.h

SOURCES += \
    $$LEARNOPENGL_ROOT_DIR/glad/src/glad.c  \
    $$LEARNOPENGL_ROOT_DIR/stb_image/stb_image.cpp  \
    main.cpp    \
    myshader.cpp  \
    mesh.cpp \
    model.cpp \
    camera.cpp

DISTFILES += \
    fragment_shader_code.frag \
    vertex_shader_code.vert \
    cubemaps.vert \
    cubemaps.frag

LIBS += -L$$LEARNOPENGL_ROOT_DIR/glfw-3.2.1/lib -lglfw3 \
        -L$$LEARNOPENGL_ROOT_DIR/assimp/lib -lassimp
