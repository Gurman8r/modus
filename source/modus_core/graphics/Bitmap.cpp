#include <modus_core/graphics/Bitmap.hpp>

#define STBI_FREE			ML_free
#define STBI_MALLOC			ML_malloc
#define STBI_REALLOC		ML_realloc
#define STBI_REALLOC_SIZED	ML_realloc_sized

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

		if (byte * const temp
		{
			stbi_load(
				path.string().c_str(),
				(int32 *)&size[0],
				(int32 *)&size[1],
				(int32 *)&channels,
				(int32)req)
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