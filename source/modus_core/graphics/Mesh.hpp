#ifndef _ML_MESH_HPP_
#define _ML_MESH_HPP_

#include <modus_core/graphics/RenderAPI.hpp>
#include <modus_core/graphics/Vertex.hpp>

namespace ml
{
	struct ML_CORE_API mesh final : trackable, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		mesh(uint32_t mode = gfx::primitive_triangles) noexcept
			: m_vao{ gfx::vertexarray::create({ mode }) }
		{
		}

		mesh(ds::list<float_t> const & v, gfx::vertex_layout const & l = {})
			: mesh{}
		{
			set_layout(l);
			add_vertices(v);
		}

		mesh(ds::list<float_t> const & v, ds::list<uint32_t> const & i, gfx::vertex_layout const & l = {})
			: mesh{}
		{
			set_layout(l);
			add_vertices(v);
			set_indices(i);
		}

		mesh(ds::list<vertex> const & verts, gfx::vertex_layout const & l = {})
			: mesh{ util::contiguous(verts), l }
		{
		}

		mesh(ds::list<vertex> const & v, ds::list<uint32_t> const & i, gfx::vertex_layout const & l = {})
			: mesh{ util::contiguous(v), i, l }
		{
		}

		mesh(fs::path const & path, gfx::vertex_layout const & l = {}) noexcept
			: mesh{ load_from_file(path), {}, l }
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

		void add_vertices(ds::list<float_t> const & value) noexcept
		{
			if (value.empty())
			{
				add_vertices(gfx::vertexbuffer::create({
					gfx::usage_dynamic,
					0 },
					nullptr));
			}
			else
			{
				add_vertices(gfx::vertexbuffer::create({
					gfx::usage_static,
					value.size() },
					value.data()));
			}
		}

		void set_layout(gfx::vertex_layout const & value) noexcept
		{
			m_vao->set_layout(value);
		}

		void set_indices(shared<gfx::indexbuffer> const & value) noexcept
		{
			m_vao->set_indices(value);
		}

		void set_indices(ds::list<uint32_t> const & value) noexcept
		{
			if (value.empty()) { set_indices(nullptr); }
			else
			{
				set_indices(gfx::indexbuffer::create({
					gfx::usage_static,
					value.size() },
					value.data()));
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static ds::list<vertex> load_from_file(fs::path const & path);

		static ds::list<vertex> load_from_file(fs::path const & path, int32_t flags);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto get_vao() const & noexcept -> shared<gfx::vertexarray> const & { return m_vao; }

		auto get_layout() const & noexcept -> gfx::vertex_layout const & { return m_vao->get_layout(); }

		auto get_indices() const & noexcept -> shared<gfx::indexbuffer> const & { return m_vao->get_indices(); }

		auto get_vertices() const & noexcept -> ds::list<shared<gfx::vertexbuffer>> const & { return m_vao->get_vertices(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		shared<gfx::vertexarray> m_vao;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_MESH_HPP_