const float SCREEN_SIZE_SCALAR = 1.0;

const vec4 verts[4] = vec4[4](
  vec4(-1.0 * SCREEN_SIZE_SCALAR, -1.0 * SCREEN_SIZE_SCALAR, 0.0, 1.0),
  vec4( 1.0 * SCREEN_SIZE_SCALAR, -1.0 * SCREEN_SIZE_SCALAR, 0.0, 1.0),
  vec4(-1.0 * SCREEN_SIZE_SCALAR,  1.0 * SCREEN_SIZE_SCALAR, 0.0, 1.0),
  vec4( 1.0 * SCREEN_SIZE_SCALAR,  1.0 * SCREEN_SIZE_SCALAR, 0.0, 1.0)
);

void main() {
  gl_Position = verts[gl_VertexID];
}
