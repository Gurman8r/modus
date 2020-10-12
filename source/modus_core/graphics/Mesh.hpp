#ifndef _ML_MESH_HPP_
#define _ML_MESH_HPP_

#include <modus_core/graphics/RenderAPI.hpp>
#include <modus_core/graphics/Vertex.hpp>

namespace ml
{
	struct ML_CORE_API mesh final : trackable, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using vertices_t	= typename pmr::vector<vertex>;
		using contiguous_t	= typename pmr::vector<float_t>;
		using indices_t		= typename pmr::vector<uint32_t>;

		static constexpr gfx::buffer_element default_layout[] =
		{
			{ vec3{}, "a_position"	},
			{ vec3{}, "a_normal"	},
			{ vec2{}, "a_texcoord"	},
		};

		mesh(uint32_t mode = gfx::primitive_triangles) noexcept
			: m_vao{ gfx::vertexarray::create(mode) }
		{
		}

		mesh(contiguous_t const & verts, gfx::buffer_layout const & layout = default_layout)
			: mesh{}
		{
			m_vao->set_layout(layout);
			add_vertices(verts);
		}

		mesh(contiguous_t const & verts, indices_t const & inds, gfx::buffer_layout const & layout = default_layout)
			: mesh{}
		{
			m_vao->set_layout(layout);
			add_vertices(verts);
			set_indices(inds);
		}

		mesh(vertices_t const & verts, gfx::buffer_layout const & layout = default_layout)
			: mesh{ util::contiguous(verts), layout }
		{
		}

		mesh(vertices_t const & verts, indices_t const & inds, gfx::buffer_layout const & layout = default_layout)
			: mesh{ util::contiguous(verts), inds, layout }
		{
		}

		mesh(fs::path const & path, gfx::buffer_layout const & layout = default_layout) noexcept
			: mesh{ load_from_file(path), {}, layout }
		{
		}

		mesh(mesh && other) noexcept : m_vao{}
		{
			swap(std::move(other));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		mesh & operator=(mesh && other) noexcept
		{
			swap(std::move(other));
			return (*this);
		}

		void swap(mesh & other) noexcept
		{
			if (this != std::addressof(other))
			{
				m_vao.swap(other.m_vao);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void add_vertices(shared<gfx::vertexbuffer> const & value) noexcept
		{
			m_vao->add_vertices(value);
		}

		void add_vertices(contiguous_t const & verts) noexcept
		{
			m_vao->add_vertices(gfx::vertexbuffer::create
			(
				verts.size(), verts.data()
			));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void set_layout(gfx::buffer_layout const & value) noexcept
		{
			m_vao->set_layout(value);
		}

		void set_indices(shared<gfx::indexbuffer> const & value) noexcept
		{
			m_vao->set_indices(value);
		}

		void set_indices(indices_t const & inds) noexcept
		{
			m_vao->set_indices(inds.empty() ? nullptr : gfx::indexbuffer::create
			(
				inds.size(), inds.data()
			));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static pmr::vector<vertex> load_from_file(fs::path const & path);

		static pmr::vector<vertex> load_from_file(fs::path const & path, int32_t flags);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto get_vao() const & noexcept -> shared<gfx::vertexarray> const & { return m_vao; }

		auto get_layout() const & noexcept -> gfx::buffer_layout const & { return m_vao->get_layout(); }

		auto get_indices() const & noexcept -> shared<gfx::indexbuffer> const & { return m_vao->get_indices(); }

		auto get_vertices() const & noexcept -> pmr::vector<shared<gfx::vertexbuffer>> const & { return m_vao->get_vertices(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		shared<gfx::vertexarray> m_vao;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_MESH_HPP_