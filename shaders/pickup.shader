#version 330

#ifdef TYPE_VERTEX
    uniform mat4 mvp;
    uniform mat4 lw;

    layout(location = 0) in vec3 vertexPosition;

    void main () {
        gl_Position = mvp * lw * vec4(vertexPosition, 1.0f);
    }
#endif

#ifdef TYPE_FRAGMENT
    uniform vec3 cubeArrayPosition;

    out vec4 fragmentColor;

    void main() {
        fragmentColor = vec4(cubeArrayPosition / 100.0f, 0.0f);
    }
#endif
