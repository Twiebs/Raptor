#pragma once

#include<Graphics\Color.h>
#include<Core\Types.h>

struct ImageData {
	uint32 width;
	uint32 height;
	RGBA8* pixels;
};
