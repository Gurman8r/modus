#ifndef _ML_MATERIAL_HPP_
#define _ML_MATERIAL_HPP_

// WIP

#include <modus_core/graphics/Shader.hpp>

namespace ml
{
	struct uniform_buffer
	{
		using storage_type = ds::list<byte>;

		storage_type m_data;

		ML_NODISCARD auto operator->() const noexcept { return const_cast<storage_type *>(&m_data); }

		ML_NODISCARD auto & operator*() const noexcept { return const_cast<storage_type &>(m_data); }

		template <class T> void write(cstring name, size_t count, T const * value)
		{
			size_t const size{ count * sizeof(T) };

			m_data.reserve(m_data.size() + size);

			std::memcpy(m_data.data(), value, size);
		}
	};

	struct material
	{
	};
}

#endif // !_ML_MATERIAL_HPP_