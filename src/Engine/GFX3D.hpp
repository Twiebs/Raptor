#ifndef RAPTOR_GFX3D_HPP
#define RAPTOR_GFX3D_HPP

enum GFX3D_RENDER_MODE {
	GFX3D_FOWARD,
	GFX3D_DEFERED,
};

namespace GFX3D {

void Init();
void Terminate();

void Begin();
void End();

};

#endif //RAPTOR_GFX3D_HPP
