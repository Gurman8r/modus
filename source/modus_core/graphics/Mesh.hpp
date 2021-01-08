#ifndef _ML_MESH_HPP_
#define _ML_MESH_HPP_

// WIP

#include <modus_core/graphics/RenderAPI.hpp>
#include <modus_core/graphics/Vertex.hpp>

namespace ml
{
	struct ML_CORE_API mesh final : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		mesh(uint32 mode = gfx::primitive_triangles) noexcept
			: m_va{ gfx::vertexarray::create({ mode }) }
		{
		}

		mesh(list<float32> const & v, gfx::buffer_layout const & l = {})
			: mesh{}
		{
			set_layout(l);
			add_vertices(v);
		}

		mesh(list<float32> const & v, list<uint32> const & i, gfx::buffer_layout const & l = {})
			: mesh{}
		{
			set_layout(l);
			add_vertices(v);
			set_indices(i);
		}

		mesh(list<vertex> const & verts, gfx::buffer_layout const & l = {})
			: mesh{ util::contiguous(verts), l }
		{
		}

		mesh(list<vertex> const & v, list<uint32> const & i, gfx::buffer_layout const & l = {})
			: mesh{ util::contiguous(v), i, l }
		{
		}

		mesh(fs::path const & path, gfx::buffer_layout const & l = {}) noexcept
			: mesh{ load_from_file(path), {}, l }
		{
		}

		mesh(mesh && other) noexcept : m_va{}
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
				m_va.swap(other.m_va);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void add_vertices(ref<gfx::vertexbuffer> const & value) noexcept
		{
			m_va->add_vertices(value);
		}

		void add_vertices(list<float32> const & value) noexcept
		{
			if (value.empty())
			{
				add_vertices(gfx::vertexbuffer::create({
					gfx::usage_dynamic,
					0u,
					nullptr }));
			}
			else
			{
				add_vertices(gfx::vertexbuffer::create({
					gfx::usage_static,
					value.size(),
					value.data() }));
			}
		}

		void set_layout(gfx::buffer_layout const & value) noexcept
		{
			m_va->set_layout(value);
		}

		void set_indices(ref<gfx::indexbuffer> const & value) noexcept
		{
			m_va->set_indices(value);
		}

		void set_indices(list<uint32> const & value) noexcept
		{
			if (value.empty()) { set_indices(nullptr); }
			else
			{
				set_indices(gfx::indexbuffer::create({
					gfx::usage_static,
					value.size(),
					value.data() }));
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static list<vertex> load_from_file(fs::path const & path);

		static list<vertex> load_from_file(fs::path const & path, int32 flags);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto get_vertexarray() const & noexcept -> ref<gfx::vertexarray> const & { return m_va; }

		auto get_layout() const & noexcept -> gfx::buffer_layout const & { return m_va->get_layout(); }

		auto get_indices() const & noexcept -> ref<gfx::indexbuffer> const & { return m_va->get_indices(); }

		auto get_vertices() const & noexcept -> list<ref<gfx::vertexbuffer>> const & { return m_va->get_vertices(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ref<gfx::vertexarray> m_va;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_MESH_HPP_