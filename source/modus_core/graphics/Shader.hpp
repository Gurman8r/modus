#ifndef _ML_SHADER_HPP_
#define _ML_SHADER_HPP_

// kind of a mess (WIP)

#include <modus_core/detail/FileUtility.hpp>
#include <modus_core/graphics/Uniform.hpp>
#include <modus_core/graphics/RenderAPI.hpp>

// SHADER ASSET
namespace ml
{
	struct shader_asset final : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		shader_asset() : m_obj{}
		{
		}

		shader_asset(fs::path const & v, fs::path const & f) : m_obj{}
		{
			load_from_file(v, f);
		}

		shader_asset(fs::path const & v, fs::path const & f, fs::path const & g) : m_obj{}
		{
			load_from_file(v, f, g);
		}

		shader_asset(ds::map<uint32_t, pmr::vector<pmr::string>> const & value) : m_obj{}
		{
			load_from_memory(value);
		}

		shader_asset(shader_asset const & other) : m_obj{}
		{
			if (other.m_obj)
			{
				load_from_memory(other.m_obj->get_source());
			}
		}

		shader_asset(shader_asset && other) noexcept : m_obj{}
		{
			swap(std::move(other));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		shader_asset & operator=(shader_asset const & other)
		{
			shader_asset temp{ other };
			swap(temp);
			return (*this);
		}

		shader_asset & operator=(shader_asset && other) noexcept
		{
			swap(std::move(other));
			return (*this);
		}

		void swap(shader_asset & value) noexcept
		{
			if (this != std::addressof(value))
			{
				m_obj.swap(value.m_obj);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool load_from_file(fs::path const & v_file, fs::path const & f_file)
		{
			return load_from_memory(
				util::get_file_string(v_file),
				util::get_file_string(f_file));
		}

		bool load_from_file(fs::path const & v_file, fs::path const & f_file, fs::path const g_file)
		{
			return load_from_memory(
				util::get_file_string(v_file),
				util::get_file_string(f_file),
				util::get_file_string(g_file));
		}

		bool load_from_memory(ds::map<uint32_t, pmr::vector<pmr::string>> const & src)
		{
			if (m_obj) { m_obj->revalue(); }
			else { m_obj = gfx::program::create(); }

			src.for_each([&](auto type, auto const & str) noexcept
			{
				m_obj->attach(type, str);
			});

			return m_obj->link();
		}

		bool load_from_memory(pmr::string const & v_src, pmr::string const & f_src)
		{
			if (m_obj) { m_obj->revalue(); }
			else { m_obj = gfx::program::create(); }

			m_obj->attach(gfx::shader_vertex, v_src);
			m_obj->attach(gfx::shader_fragment, f_src);

			return m_obj->link();
		}

		bool load_from_memory(pmr::string const & v_src, pmr::string const & f_src, pmr::string const & g_src)
		{
			if (m_obj) { m_obj->revalue(); }
			else { m_obj = gfx::program::create(); }

			m_obj->attach(gfx::shader_vertex	, v_src);
			m_obj->attach(gfx::shader_fragment	, f_src);
			m_obj->attach(gfx::shader_geometry	, g_src);

			return m_obj->link();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void bind() const noexcept { m_obj->bind(); }

		void unbind() const noexcept { m_obj->unbind(); }

		void bind_textures() const noexcept { m_obj->bind_textures(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T
		> bool set_uniform(cstring name, T && value) noexcept
		{
			return m_obj->set_uniform(name, ML_forward(value));
		}

		template <class T
		> bool set_uniform(pmr::string const & name, T && value) noexcept
		{
			return this->set_uniform(name.c_str(), ML_forward(value));
		}

		bool set_uniform(uniform const & u) noexcept
		{
			switch (u.name().empty() ? 0 : u.type().hash())
			{
			default							: return false;
			case hashof_v<bool>				: return set_uniform(u.name(), *u.get<bool>());
			case hashof_v<int32_t>			: return set_uniform(u.name(), *u.get<int32_t>());
			case hashof_v<float_t>			: return set_uniform(u.name(), *u.get<float_t>());
			case hashof_v<vec2>				: return set_uniform(u.name(), *u.get<vec2>());
			case hashof_v<vec3>				: return set_uniform(u.name(), *u.get<vec3>());
			case hashof_v<vec4>				: return set_uniform(u.name(), *u.get<vec4>());
			case hashof_v<color>			: return set_uniform(u.name(), *u.get<color>());
			case hashof_v<mat2>				: return set_uniform(u.name(), *u.get<mat2>());
			case hashof_v<mat3>				: return set_uniform(u.name(), *u.get<mat3>());
			case hashof_v<mat4>				: return set_uniform(u.name(), *u.get<mat4>());
			case hashof_v<gfx::texture>		:
			case hashof_v<gfx::texture2d>	:
			case hashof_v<gfx::texturecube>	: return set_uniform(u.name(), *u.get<gfx::texture>());
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto program() const & noexcept -> shared<gfx::program> const & { return m_obj; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		shared<gfx::program> m_obj;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// SHADER CACHE
namespace ml
{
	struct shader_cache final : trackable, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		using source_storage = ds::array<ds::map<pmr::string, pmr::string>, gfx::shader_MAX>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		shader_cache(allocator_type alloc = {})
			: m_src{ alloc }
		{
		}

		shader_cache(shader_cache && other, allocator_type alloc = {}) noexcept
			: shader_cache{ alloc }
		{
			this->swap(std::move(other));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		shader_cache & operator=(shader_cache && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(shader_cache & other) noexcept
		{
			if (this != std::addressof(other))
			{
				m_src.swap(other.m_src);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto & get(uint32_t i) & noexcept { return m_src[static_cast<size_t>(i)]; }

		auto const & get(uint32_t i) const & noexcept { return m_src[static_cast<size_t>(i)]; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		pmr::string & str(uint32_t i, pmr::string const & name) & noexcept
		{
			return this->get(i)[name];
		}

		cstring c_str(uint32_t i, pmr::string const & name) noexcept
		{
			return str(i, name).c_str();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool read_file(uint32_t i, pmr::string const & name, fs::path const & path)
		{
			if (auto const contents{ util::get_file_contents(path) })
			{
				this->str(i, name) = { contents->begin(), contents->end() };

				return true;
			}
			return false;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		source_storage m_src;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_SHADER_HPP_