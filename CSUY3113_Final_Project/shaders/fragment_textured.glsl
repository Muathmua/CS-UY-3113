uniform sampler2D diffuse;
varying vec2 texCoordVar;
uniform int effect_toggle;

void main() {
    vec4 colour = texture2D(diffuse, texCoordVar);

    if (effect_toggle == 1) {
        gl_FragColor = vec4(colour.r, colour.g, colour.b + 0.1, colour.a);
  
    } else {
        gl_FragColor = colour;
    }
}
