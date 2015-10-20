#ifndef RAPTOR_GFX3D_HPP
#define RAPTOR_GFX3D_HPP


#include <Math/Matrix4.hpp>
#include <Graphics/Render3D.hpp>
using namespace Raptor;

#define MODEL_MATRIX_LOCATION 0
#define VIEW_MATRIX_LOCATION 1
#define PROJECTION_MATRIX_LOCATION 2
#define CAMERA_POSITION_LOCATION 3

inline void SetVector3Uniform (GLint location, const Vector3& vector);
inline void SetCameraUniforms (const Camera& camera);
inline void SetMatrix4Uniform (U32 matrixLocation, const Matrix4& matrix);

inline void SetVector3Uniform (GLint location, const Vector3& vector) {
	glUniform3fv(location, 1, &vector.x);
}

inline void SetCameraUniforms (const Camera& camera) {
	SetVector3Uniform(CAMERA_POSITION_LOCATION, camera.position);
	SetMatrix4Uniform(VIEW_MATRIX_LOCATION, camera.view);
	SetMatrix4Uniform(PROJECTION_MATRIX_LOCATION, camera.projection);
}

inline void SetMatrix4Uniform (U32 matrixLocation, const Matrix4& matrix) {
	glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, &matrix[0][0]);
}

enum GFX3D_RENDER_MODE {
	GFX3D_FOWARD,
	GFX3D_DEFERED,
};

namespace GFX3D {

void Init();
void Terminate();

void Begin();
void End();

void SetProjectionMatrix (const Matrix4& matrix);


};

#endif // RAPTOR_GFX3D_HPP
