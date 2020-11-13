#include <modus_core/graphics/Bitmap.hpp>

#define STBI_MALLOC(s)				ml::ml_malloc(s)
#define STBI_FREE(p)				ml::ml_free(p)
#define STBI_REALLOC(p, s)			ml::ml_realloc(p, s)
#define STBI_REALLOC_SIZED(p, o, n) ml::ml_realloc(p, o, n)

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	bool bitmap::load_from_file(
		fs::path const	&	path,
		pixels			&	pix,
		vec2s			&	size,
		size_t			&	channels,
		bool				flip_v,
		size_t				req)
	{
		pix.clear();
		size = {};
		channels = 0;
		
		if (path.empty()) { return false; }

		stbi_set_flip_vertically_on_load(flip_v);

		if (byte_t * const temp
		{
			stbi_load(
				path.string().c_str(),
				(int32_t *)&size[0],
				(int32_t *)&size[1],
				(int32_t *)&channels,
				(int32_t)req)
		}
		; !temp) { return false; }
		else
		{
			pix = { temp, temp + size[0] * size[1] * channels };

			stbi_image_free(temp);

			return true;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}