#if defined(ML_IMPL_RENDERER_OPENGL)
#ifndef _ML_OPENGL_RENDER_API_HPP_
#define _ML_OPENGL_RENDER_API_HPP_

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <modus_core/graphics/RenderAPI.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// render device
namespace ml::gfx
{
	struct opengl_render_device final : render_device
	{
	private:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr typeof<> s_self_type{ typeof_v<opengl_render_device> };

		device_info				m_info	{}; // device settings
		ds::ref<render_context>	m_ctx	{}; // render context

	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		opengl_render_device(allocator_type alloc);

		~opengl_render_device() override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ds::ref<render_context> const & get_context() const noexcept override { return m_ctx; }

		ds::ref<render_context> & set_context(ds::ref<render_context> const & value) noexcept override { return m_ctx = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		object_id get_handle() const noexcept override { return ML_handle(object_id, this); }

		device_info const & get_info() const noexcept override { return m_info; }

		typeof<> const & get_self_type() const noexcept override { return s_self_type; }

	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ds::ref<render_context> new_context(spec<render_context> const & desc, allocator_type alloc) noexcept override;

		ds::ref<vertexarray> new_vertexarray(spec<vertexarray> const & desc, allocator_type alloc) noexcept override;

		ds::ref<vertexbuffer> new_vertexbuffer(spec<vertexbuffer> const & desc, addr_t data, allocator_type alloc) noexcept override;

		ds::ref<indexbuffer> new_indexbuffer(spec<indexbuffer> const & desc, addr_t data, allocator_type alloc) noexcept override;

		ds::ref<texture2d> new_texture2d(spec<texture2d> const & desc, addr_t data, allocator_type alloc) noexcept override;

		ds::ref<texturecube> new_texturecube(spec<texturecube> const & desc, allocator_type alloc) noexcept override;

		ds::ref<framebuffer> new_framebuffer(spec<framebuffer> const & desc, allocator_type alloc) noexcept override;

		ds::ref<program> new_program(spec<program> const & desc, allocator_type alloc) noexcept override;

		ds::ref<shader> new_shader(spec<shader> const & desc, allocator_type alloc) noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// render context
namespace ml::gfx
{
	// opengl context
	struct opengl_render_context final : render_context
	{
	private:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr typeof<> s_self_type{ typeof_v<opengl_render_context> };

		spec		m_data		{}; // context settings
		uint32_t	m_handle	{}; // pipeline handle (WIP)

	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		opengl_render_context(render_device * parent, spec const & desc, allocator_type alloc);

		~opengl_render_context() override = default;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		object_id get_handle() const noexcept override { return ML_handle(object_id, m_handle); }

		typeof<> const & get_self_type() const noexcept override { return s_self_type; }

		spec const & get_spec() const noexcept override { return m_data; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		alpha_state * get_alpha_state(alpha_state * value = {}) const override;
		
		blend_state * get_blend_state(blend_state * value = {}) const override;

		color * get_clear_color(color * value = {}) const override;
		
		cull_state * get_cull_state(cull_state * value = {}) const override;

		depth_state * get_depth_state(depth_state * value = {}) const override;

		stencil_state * get_stencil_state(stencil_state * value = {}) const override;

		int_rect * get_viewport(int_rect * value = {}) const override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void set_alpha_state(alpha_state const & value) override;
		
		void set_blend_state(blend_state const & value) override;

		void set_clear_color(color const & value) override;
		
		void set_cull_state(cull_state const & value) override;

		void set_depth_state(depth_state const & value) override;

		void set_stencil_state(stencil_state const & value) override;

		void set_viewport(int_rect const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void clear(uint32_t mask) override;

		void draw(ds::ref<vertexarray> const & value) override;

		void draw_arrays(uint32_t prim, size_t first, size_t count) override;

		void draw_indexed(uint32_t prim, size_t count) override;

		void flush() override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void bind_vertexarray(vertexarray const * value) override;

		void bind_vertexbuffer(vertexbuffer const * value) override;

		void bind_indexbuffer(indexbuffer const * value) override;

		void bind_texture(texture const * value, uint32_t slot = 0) override;

		void bind_framebuffer(framebuffer const * value) override;

		void bind_program(program const * value) override;

		void bind_shader(shader const * value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void upload(uniform_id loc, bool value) override;

		void upload(uniform_id loc, int32_t value) override;

		void upload(uniform_id loc, float_t value) override;

		void upload(uniform_id loc, vec2f const & value) override;

		void upload(uniform_id loc, vec3f const & value) override;

		void upload(uniform_id loc, vec4f const & value) override;

		void upload(uniform_id loc, mat2f const & value) override;

		void upload(uniform_id loc, mat3f const & value)  override;

		void upload(uniform_id loc, mat4f const & value) override;
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// vertexarray
namespace ml::gfx
{
	// opengl vertexarray
	struct opengl_vertexarray final : vertexarray
	{
	private:
		static constexpr typeof<> s_self_type{ typeof_v<opengl_vertexarray> };

		uint32_t						m_handle	{}; // handle
		vertex_layout					m_layout	{}; // buffer layout
		uint32_t const					m_mode		{}; // prim type
		ds::ref<indexbuffer>			m_indices	{}; // index buffer
		ds::list<ds::ref<vertexbuffer>>	m_vertices	{}; // vertex buffers

	public:
		opengl_vertexarray(render_device * parent, spec const & desc, allocator_type alloc);

		~opengl_vertexarray() override;

		bool revalue() override;

		object_id get_handle() const noexcept override { return ML_handle(object_id, m_handle); }

		typeof<> const & get_self_type() const noexcept override { return s_self_type; }

	public:
		void add_vertices(ds::ref<vertexbuffer> const & value) override;

		void set_layout(vertex_layout const & value) override { m_layout = value; }

		void set_indices(ds::ref<indexbuffer> const & value) override;

		vertex_layout const & get_layout() const noexcept override { return m_layout; }

		ds::ref<indexbuffer> const & get_indices() const noexcept override { return m_indices; }

		uint32_t get_mode() const noexcept override { return m_mode; }

		ds::list<ds::ref<vertexbuffer>> const & get_vertices() const noexcept override { return m_vertices; }
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// vertexbuffer
namespace ml::gfx
{
	// opengl vertexbuffer
	struct opengl_vertexbuffer final : vertexbuffer
	{
	private:
		static constexpr typeof<> s_self_type{ typeof_v<opengl_vertexbuffer> };

		uint32_t		m_handle	{}; // handle
		uint32_t const	m_usage		{}; // draw usage
		buffer_t		m_buffer	{}; // local data

	public:
		opengl_vertexbuffer(render_device * parent, spec const & desc, addr_t data, allocator_type alloc);

		~opengl_vertexbuffer() override;

		bool revalue() override;

		object_id get_handle() const noexcept override { return ML_handle(object_id, m_handle); }

		typeof<> const & get_self_type() const noexcept override { return s_self_type; }

	public:
		void set_data(size_t count, addr_t data, size_t offset = 0) override;

		buffer_t const & get_buffer() const noexcept override { return m_buffer; }

		size_t get_count() const noexcept override { return m_buffer.size() / sizeof(float_t); }
		
		size_t get_size() const noexcept override { return m_buffer.size(); }

		uint32_t get_usage() const noexcept override { return m_usage; }
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// indexbuffer
namespace ml::gfx
{
	// opengl indexbuffer
	struct opengl_indexbuffer final : indexbuffer
	{
	private:
		static constexpr typeof<> s_self_type{ typeof_v<opengl_indexbuffer> };

		uint32_t		m_handle	{}; // handle
		uint32_t const	m_usage		{}; // usage
		buffer_t		m_buffer	{}; // local data

	public:
		opengl_indexbuffer(render_device * parent, spec const & desc, addr_t data, allocator_type alloc);

		~opengl_indexbuffer() override;

		bool revalue() override;

		object_id get_handle() const noexcept override { return ML_handle(object_id, m_handle); }

		typeof<> const & get_self_type() const noexcept override { return s_self_type; }

	public:
		void set_data(size_t count, addr_t data, size_t offset = 0) override;

		buffer_t const & get_buffer() const noexcept override { return m_buffer; }

		size_t get_count() const noexcept override { return m_buffer.size() / sizeof(uint32_t); }

		size_t get_size() const noexcept override { return m_buffer.size(); }

		uint32_t get_usage() const noexcept override { return m_usage; }
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// texture2d
namespace ml::gfx
{
	// opengl texture2d
	struct opengl_texture2d final : texture2d
	{
	private:
		static constexpr typeof<> s_self_type{ typeof_v<opengl_texture2d> };

		vec2i			m_size		{}			; // 
		texture_format	m_format	{}			; // 
		texture_flags_	m_flags		{}			; // 
		uint32_t		m_handle	{}			; // handle
		bool			m_locked	{ true }	; // locked

	public:
		opengl_texture2d(render_device * parent, spec const & desc, addr_t data, allocator_type alloc);

		~opengl_texture2d() override;

		bool revalue() override;

		object_id get_handle() const noexcept override { return ML_handle(object_id, m_handle); }

		typeof<> const & get_self_type() const noexcept override { return s_self_type; }

	public:
		void lock() override;

		void unlock() override;

		void update(vec2i const & size, addr_t data = {}) override;

		void update(vec2i const & pos, vec2i const & size, addr_t data) override;

		void set_mipmapped(bool value) override;

		void set_repeated(bool value) override;

		void set_smooth(bool value) override;

		bitmap copy_to_image() const override;

		vec2i const & get_size() const noexcept { return m_size; }

		texture_format const & get_format() const noexcept { return m_format; }

		texture_flags_ get_flags() const noexcept { return m_flags; }
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// texturecube
namespace ml::gfx
{
	// opengl texturecube
	struct opengl_texturecube : texturecube
	{
	private:
		static constexpr typeof<> s_self_type{ typeof_v<opengl_texturecube> };

		vec2i			m_size		{}			; // 
		texture_format	m_format	{}			; // 
		texture_flags_	m_flags		{}			; // 
		uint32_t		m_handle	{}			; // handle
		bool			m_locked	{ true }	; // locked

	public:
		opengl_texturecube(render_device * parent, spec const & desc, allocator_type alloc);

		~opengl_texturecube() override;

		bool revalue() override;

		object_id get_handle() const noexcept override { return ML_handle(object_id, m_handle); }

		typeof<> const & get_self_type() const noexcept override { return s_self_type; }

	public:
		void lock() override;

		void unlock() override;

		vec2i const & get_size() const noexcept { return m_size; }

		texture_format const & get_format() const noexcept { return m_format; }

		texture_flags_ get_flags() const noexcept { return m_flags; }
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// framebuffer
namespace ml::gfx
{
	// opengl framebuffer
	struct opengl_framebuffer final : framebuffer
	{
	private:
		static constexpr typeof<> s_self_type{ typeof_v<opengl_framebuffer> };

		vec2i							m_size			{}; // 
		texture_format					m_format		{}; // 
		texture_flags_					m_flags			{}; // 
		vec4i							m_bpp			{}; // 
		int32_t							m_stencil_bits	{}; // 
		int32_t							m_depth_bits	{}; // 
		int32_t							m_samples		{}; // 
		bool							m_stereo		{}; // 
		uint32_t						m_handle		{}; // handle
		ds::list<ds::ref<texture2d>>	m_attachments	{}; // color attachments
		ds::ref<texture2d>				m_depth			{}; // depth attachment

		
	public:
		opengl_framebuffer(render_device * parent, spec const & desc, allocator_type alloc);

		~opengl_framebuffer() override;

		bool revalue() override;

		object_id get_handle() const noexcept override { return ML_handle(object_id, m_handle); }

		typeof<> const & get_self_type() const noexcept override { return s_self_type; }

	public:
		bool attach(ds::ref<texture2d> const & value) override;

		bool detach(ds::ref<texture2d> const & value) override;

		void resize(vec2i const & value) override;

		ds::list<ds::ref<texture2d>> const & get_color_attachments() const noexcept override { return m_attachments; }

		ds::ref<texture2d> const & get_depth_attachment() const noexcept override { return m_depth; }

		vec2i const & get_size() const noexcept { return m_size; }

		texture_format const & get_format() const noexcept { return m_format; }

		texture_flags_ get_flags() const noexcept { return m_flags; }

		vec4i const & get_bits_per_pixel() const noexcept override { return m_bpp; }

		int32_t get_stencil_bits() const noexcept override { return m_stencil_bits; }

		int32_t get_depth_bits() const noexcept override { return m_depth_bits; }

		int32_t get_samples() const noexcept override { return m_samples; }

		bool is_stereo() const noexcept override { return m_stereo; }
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// program
namespace ml::gfx
{
	// opengl program
	struct opengl_program final : program
	{
	private:
		static constexpr typeof<> s_self_type{ typeof_v<opengl_program> };

		uint32_t								m_handle		{}; // handle
		ds::string								m_error_log		{}; // error log
		ds::map<uint32_t, object_id>			m_shaders		{}; // shader cache
		ds::map<uint32_t, ds::list<ds::string>>	m_source		{}; // source cache
		ds::map<uniform_id, ds::ref<texture>>	m_textures		{}; // texture cache
		ds::map<hash_t, uniform_id>				m_uniforms		{}; // uniform cache

		// uniform binder
		struct ML_NODISCARD program_uniform_binder final
		{
			uniform_id location{ ML_handle(uniform_id, -1) };

			uint32_t self{}, last{};

			operator bool() const noexcept { return -1 < ML_handle(int32_t, location); }

			program_uniform_binder(opengl_program & s, cstring name) noexcept;

			~program_uniform_binder() noexcept;
		};

	public:
		opengl_program(render_device * parent, spec const & desc, allocator_type alloc);

		~opengl_program() override;

		bool revalue() override;

		object_id get_handle() const noexcept override { return ML_handle(object_id, m_handle); }

		typeof<> const & get_self_type() const noexcept override { return s_self_type; }

	public:
		bool attach(uint32_t type, size_t count, cstring * str, int32_t const * len = {}) override;

		bool detach(uint32_t type) override;

		bool link() override;

		bool bind_uniform(cstring name, ds::method<void(uniform_id)> const & fn) override
		{
			program_uniform_binder u{ *this, name };
			if (u) { std::invoke(fn, u.location); }
			return u;
		}

		ds::string const & get_error_log() const noexcept override { return m_error_log; }

		ds::map<uint32_t, object_id> const & get_shaders() const noexcept override { return m_shaders; }

		ds::map<uint32_t, ds::list<ds::string>> const & get_source() const noexcept override { return m_source; }

		ds::map<uniform_id, ds::ref<texture>> const & get_textures() const noexcept override { return m_textures; }

		ds::map<hash_t, uniform_id> const & get_uniforms() const noexcept override { return m_uniforms; }

	public:
		void do_cache_texture(uniform_id loc, ds::ref<texture> const & value) noexcept override
		{
			static auto const max_texture_slots
			{
				(size_t)get_device()->get_info().max_texture_slots
			};
			if (auto const it{ m_textures.find(loc) })
			{
				(*it->second) = value;
			}
			else if ((m_textures.size() + 1) < max_texture_slots)
			{
				m_textures.insert(loc, value);
			}
		}
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// shader
namespace ml::gfx
{
	// opengl shader
	struct opengl_shader final : shader
	{
	private:
		static constexpr typeof<> s_self_type{ typeof_v<opengl_shader> };

		uint32_t								m_type		{}; // type
		ds::list<ds::string>					m_code		{}; // code
		uint32_t								m_handle	{}; // handle
		ds::string								m_log		{}; // error log
		ds::list<ds::string>					m_source	{}; // source
		ds::map<hash_t, uniform_id>				m_attribs	{}; // attributes
		ds::map<hash_t, uniform_id>				m_uniforms	{}; // uniforms
		ds::map<uniform_id, ds::ref<texture>>	m_textures	{}; // textures

		struct ML_NODISCARD shader_uniform_binder final
		{
			uniform_id loc{ (uniform_id)-1 };

			uint32_t self{}, last{};

			operator bool() const noexcept { return -1 < ML_handle(int32_t, loc); }

			shader_uniform_binder(opengl_shader & s, cstring name) noexcept;

			~shader_uniform_binder() noexcept;
		};

	public:
		opengl_shader(render_device * parent, spec const & desc, allocator_type alloc);

		~opengl_shader() override;

		object_id get_handle() const noexcept override { return ML_handle(object_id, m_handle); }

		typeof<> const & get_self_type() const noexcept override { return s_self_type; }

	public:
		bool compile(uint32_t type, size_t count, cstring * str, int32_t const * len = {}) override;

		bool bind_uniform(cstring name, ds::method<void(uniform_id)> const & fn) override
		{
			shader_uniform_binder u{ *this, name };
			if (u) { std::invoke(fn, u.loc); }
			return u;
		}

		ds::string const & get_info_log() const noexcept override { return m_log; }

		ds::list<ds::string> const & get_source() const noexcept override { return m_source; }

		ds::map<uniform_id, ds::ref<texture>> const & get_textures() const noexcept override { return m_textures; }

		uint32_t get_type() const noexcept override { return m_type; }

	protected:
		void do_cache(uniform_id loc, ds::ref<texture> const & value) override
		{
			static auto const max_texture_slots
			{
				(size_t)get_device()->get_info().max_texture_slots
			};
			if (auto const it{ m_textures.find(loc) })
			{
				(*it->second) = value;
			}
			else if ((m_textures.size() + 1) < max_texture_slots)
			{
				m_textures.insert(loc, value);
			}
		}

		void do_upload(uniform_id loc, bool value) override;

		void do_upload(uniform_id loc, int32_t value) override;

		void do_upload(uniform_id loc, uint32_t value) override;

		void do_upload(uniform_id loc, float_t value) override;

		void do_upload(uniform_id loc, vec2f const & value) override;

		void do_upload(uniform_id loc, vec3f const & value) override;

		void do_upload(uniform_id loc, vec4f const & value) override;

		void do_upload(uniform_id loc, mat2f const & value, bool transpose = false) override;

		void do_upload(uniform_id loc, mat3f const & value, bool transpose = false) override;

		void do_upload(uniform_id loc, mat4f const & value, bool transpose = false) override;

		void do_upload(uniform_id loc, ds::ref<texture> const & value, uint32_t slot = 0) override;
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_IMPL_RENDERER_OPENGL_HPP_
#endif // _ML_OPENGL_RENDER_API_HPP_