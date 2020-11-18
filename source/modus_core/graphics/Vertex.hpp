#ifndef _ML_VERTEX_HPP_
#define _ML_VERTEX_HPP_

#include <modus_core/detail/Matrix.hpp>

namespace ml
{
	struct vertex final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr size_t size{ 8 };

		using storage_type = typename ds::array<float32, size>;

		explicit vertex(vec3 const & p, vec3 const & n, vec2 const & t) : m_data{
			p[0], p[1], p[2], n[0], n[1], n[2], t[0], t[1]
		}
		{
		}

		vertex(std::initializer_list<float32> init) : m_data{}
		{
			for (auto it{ init.begin() }; it != init.end(); ++it)
			{
				if (auto const i{ (size_t)std::distance(init.begin(), it) }; i < size)
				{
					m_data[i] = (*it);
				}
			}
		}

		vertex(storage_type const & storage) : m_data{ storage }
		{
		}

		vertex(storage_type && storage) noexcept : m_data{ std::move(storage) }
		{
		}

		vertex(vertex const & value) : m_data{ value.m_data }
		{
		}

		vertex(vertex && value) noexcept : m_data{}
		{
			swap(std::move(value));
		}

		vertex() noexcept : m_data{}
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		vertex & operator=(vertex const & value)
		{
			vertex temp{ value };
			swap(temp);
			return (*this);
		}

		vertex & operator=(vertex && value) noexcept
		{
			swap(std::move(value));
			return (*this);
		}

		void swap(vertex & value) noexcept
		{
			if (this != std::addressof(value))
			{
				util::swap(m_data, value.m_data);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto operator[](size_t const i) & noexcept -> float32 & { return m_data[i]; }
		
		auto operator[](size_t const i) const & noexcept -> float32 const & { return m_data[i]; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD vec3 const & position() const noexcept
		{
			return *reinterpret_cast<vec3 const *>(&m_data[0]);
		}

		ML_NODISCARD vec3 const & normal() const noexcept
		{
			return *reinterpret_cast<vec3 const *>(&m_data[3]);
		}

		ML_NODISCARD vec2 const & texcoord() const noexcept
		{
			return *reinterpret_cast<vec2 const *>(&m_data[6]);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		vec3 & position() noexcept
		{
			return *reinterpret_cast<vec3 *>(&m_data[0]);
		}

		vec3 & normal() noexcept
		{
			return *reinterpret_cast<vec3 *>(&m_data[3]);
		}

		vec2 & texcoord() noexcept
		{
			return *reinterpret_cast<vec2 *>(&m_data[6]);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		storage_type m_data;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

namespace ml::util
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_NODISCARD inline auto contiguous(ds::list<vertex> const & v) noexcept
	{
		ds::list<float32> temp{};
		if (size_t const imax{ v.size() * vertex::size })
		{
			temp.reserve(imax);
			for (size_t i = 0; i < imax; ++i)
			{
				temp.push_back(v[i / vertex::size][i % vertex::size]);
			}
		}
		return temp;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_VERTEX_HPP_