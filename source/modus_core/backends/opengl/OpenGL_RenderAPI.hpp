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
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		static constexpr typeof_t<> s_self_type{ typeof_v<opengl_render_device> };

		device_info m_info{}; // device info

		ref<render_context> m_context{}; // active context

		ds::batch_vector // all objects
		<
			unown<render_context>,
			unown<vertexarray>,
			unown<vertexbuffer>,
			unown<indexbuffer>,
			unown<texture2d>,
			unown<texture3d>,
			unown<texturecube>,
			unown<framebuffer>,
			unown<program>,
			unown<shader>
		>
		m_objs{};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		opengl_render_device(spec_type const & desc, allocator_type alloc);

		~opengl_render_device() final;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		object_id get_handle() const noexcept final { return ML_handle(object_id, this); }

		device_info const & get_info() const noexcept final { return m_info; }

		typeof_t<> const & get_self_type() const noexcept final { return s_self_type; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ref<render_context> const & get_context() const noexcept final { return m_context; }

		void set_context(ref<render_context> const & value) noexcept final;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ref<render_context> new_context(spec<render_context> const & desc, allocator_type alloc) noexcept final;

		ref<vertexarray> new_vertexarray(spec<vertexarray> const & desc, allocator_type alloc) noexcept final;

		ref<vertexbuffer> new_vertexbuffer(spec<vertexbuffer> const & desc, allocator_type alloc) noexcept final;

		ref<indexbuffer> new_indexbuffer(spec<indexbuffer> const & desc, allocator_type alloc) noexcept final;

		ref<texture2d> new_texture2d(spec<texture2d> const & desc, allocator_type alloc) noexcept final;
		
		ref<texture3d> new_texture3d(spec<texture3d> const & desc, allocator_type alloc = {}) noexcept final;

		ref<texturecube> new_texturecube(spec<texturecube> const & desc, allocator_type alloc) noexcept final;

		ref<framebuffer> new_framebuffer(spec<framebuffer> const & desc, allocator_type alloc) noexcept final;

		ref<program> new_program(spec<program> const & desc, allocator_type alloc) noexcept final;

		ref<shader> new_shader(spec<shader> const & desc, allocator_type alloc) noexcept final;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ds::list<unown<render_context>> const & all_contexts() const noexcept final { return m_objs.get<unown<render_context>>(); }

		ds::list<unown<vertexarray>> const & all_vertexarrays() const noexcept { return m_objs.get<unown<vertexarray>>(); }

		ds::list<unown<vertexbuffer>> const & all_vertexbuffers() const noexcept { return m_objs.get<unown<vertexbuffer>>(); }

		ds::list<unown<indexbuffer>> const & all_indexbuffers() const noexcept { return m_objs.get<unown<indexbuffer>>(); }

		ds::list<unown<texture2d>> const & all_texture2ds() const noexcept { return m_objs.get<unown<texture2d>>(); }

		ds::list<unown<texture3d>> const & all_texture3ds() const noexcept { return m_objs.get<unown<texture3d>>(); }

		ds::list<unown<texturecube>> const & all_texturecubes() const noexcept { return m_objs.get<unown<texturecube>>(); }

		ds::list<unown<framebuffer>> const & all_framebuffers() const noexcept { return m_objs.get<unown<framebuffer>>(); }

		ds::list<unown<program>> const & all_programs() const noexcept { return m_objs.get<unown<program>>(); }

		ds::list<unown<shader>> const & all_shaders() const noexcept { return m_objs.get<unown<shader>>(); }

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
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		static constexpr typeof_t<> s_self_type{ typeof_v<opengl_render_context> };

		uint32		m_handle	{}; // pipeline handle (WIP)
		spec_type	m_desc		{}; // context settings

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
	public:
		opengl_render_context(render_device * parent, spec_type const & desc, allocator_type alloc);

		~opengl_render_context() final;

		bool revalue() final;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		object_id get_handle() const noexcept final { return ML_handle(object_id, m_handle); }

		typeof_t<> const & get_self_type() const noexcept final { return s_self_type; }

		spec_type const & get_spec() const noexcept final { return m_desc; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		alpha_state * get_alpha_state(alpha_state * value = {}) const final;
		
		blend_state * get_blend_state(blend_state * value = {}) const final;

		color * get_clear_color(color * value = {}) const final;
		
		cull_state * get_cull_state(cull_state * value = {}) const final;

		depth_state * get_depth_state(depth_state * value = {}) const final;

		stencil_state * get_stencil_state(stencil_state * value = {}) const final;

		int_rect * get_viewport(int_rect * value = {}) const final;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void set_alpha_state(alpha_state const & value) final;
		
		void set_blend_state(blend_state const & value) final;

		void set_clear_color(color const & value) final;
		
		void set_cull_state(cull_state const & value) final;

		void set_depth_state(depth_state const & value) final;

		void set_stencil_state(stencil_state const & value) final;

		void set_viewport(int_rect const & value) final;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void clear(uint32 mask) final;

		void draw(ref<vertexarray> const & value) final;

		void draw_arrays(uint32 prim, size_t first, size_t count) final;

		void draw_indexed(uint32 prim, size_t count) final;

		void flush() final;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void bind_vertexarray(vertexarray const * value) final;

		void bind_vertexbuffer(vertexbuffer const * value) final;

		void bind_indexbuffer(indexbuffer const * value) final;

		void bind_texture(texture const * value, uint32 slot = 0) final;

		void bind_framebuffer(framebuffer const * value) final;

		void bind_program(program const * value) final;
		
		void bind_shader(shader const * value) final;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void upload(uniform_id loc, bool value) final;

		void upload(uniform_id loc, int32 value) final;

		void upload(uniform_id loc, float32 value) final;

		void upload(uniform_id loc, vec2f const & value) final;

		void upload(uniform_id loc, vec3f const & value) final;

		void upload(uniform_id loc, vec4f const & value) final;

		void upload(uniform_id loc, mat2f const & value) final;

		void upload(uniform_id loc, mat3f const & value)  final;

		void upload(uniform_id loc, mat4f const & value) final;
		
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
		static constexpr typeof_t<> s_self_type{ typeof_v<opengl_vertexarray> };

		uint32							m_handle	{}; // handle
		buffer_layout					m_layout	{}; // buffer layout
		uint32 const					m_mode		{}; // prim type
		ref<indexbuffer>			m_indices	{}; // index buffer
		ds::list<ref<vertexbuffer>>	m_vertices	{}; // vertex buffers

	public:
		opengl_vertexarray(render_device * parent, spec_type const & desc, allocator_type alloc);

		~opengl_vertexarray() final;

		bool revalue() final;

		object_id get_handle() const noexcept final { return ML_handle(object_id, m_handle); }

		typeof_t<> const & get_self_type() const noexcept final { return s_self_type; }

	public:
		void add_vertices(ref<vertexbuffer> const & value) final;

		void set_layout(buffer_layout const & value) final { m_layout = value; }

		void set_indices(ref<indexbuffer> const & value) final;

		buffer_layout const & get_layout() const noexcept final { return m_layout; }

		ref<indexbuffer> const & get_indices() const noexcept final { return m_indices; }

		uint32 get_mode() const noexcept final { return m_mode; }

		ds::list<ref<vertexbuffer>> const & get_vertices() const noexcept final { return m_vertices; }
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
		static constexpr typeof_t<> s_self_type{ typeof_v<opengl_vertexbuffer> };

		uint32			m_handle	{}; // handle
		uint32 const	m_usage		{}; // draw usage
		buffer_t		m_buffer	{}; // local data

	public:
		opengl_vertexbuffer(render_device * parent, spec_type const & desc, allocator_type alloc);

		~opengl_vertexbuffer() final;

		bool revalue() final;

		object_id get_handle() const noexcept final { return ML_handle(object_id, m_handle); }

		typeof_t<> const & get_self_type() const noexcept final { return s_self_type; }

	public:
		void set_data(size_t count, addr_t data, size_t offset = 0) final;

		buffer_t const & get_buffer() const noexcept final { return m_buffer; }

		size_t get_count() const noexcept final { return m_buffer.size() / sizeof(float32); }
		
		size_t get_size() const noexcept final { return m_buffer.size(); }

		uint32 get_usage() const noexcept final { return m_usage; }
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
		static constexpr typeof_t<> s_self_type{ typeof_v<opengl_indexbuffer> };

		uint32			m_handle	{}; // handle
		uint32 const	m_usage		{}; // usage
		buffer_t		m_buffer	{}; // local data

	public:
		opengl_indexbuffer(render_device * parent, spec_type const & desc, allocator_type alloc);

		~opengl_indexbuffer() final;

		bool revalue() final;

		object_id get_handle() const noexcept final { return ML_handle(object_id, m_handle); }

		typeof_t<> const & get_self_type() const noexcept final { return s_self_type; }

	public:
		void set_data(size_t count, addr_t data, size_t offset = 0) final;

		buffer_t const & get_buffer() const noexcept final { return m_buffer; }

		size_t get_count() const noexcept final { return m_buffer.size() / sizeof(uint32); }

		size_t get_size() const noexcept final { return m_buffer.size(); }

		uint32 get_usage() const noexcept final { return m_usage; }
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
		static constexpr typeof_t<> s_self_type{ typeof_v<opengl_texture2d> };

		vec2i			m_size		{}			; // 
		texture_format	m_format	{}			; // 
		texture_flags_	m_flags		{}			; // 
		uint32			m_handle	{}			; // handle
		bool			m_locked	{ true }	; // locked

	public:
		opengl_texture2d(render_device * parent, spec_type const & desc, allocator_type alloc);

		~opengl_texture2d() final;

		bool revalue() final;

		object_id get_handle() const noexcept final { return ML_handle(object_id, m_handle); }

		typeof_t<> const & get_self_type() const noexcept final { return s_self_type; }

	public:
		void lock() final;

		void unlock() final;

		void update(vec2i const & size, addr_t data = {}) final;

		void update(vec2i const & pos, vec2i const & size, addr_t data) final;

		void set_mipmapped(bool value) final;

		void set_repeated(bool value) final;

		void set_smooth(bool value) final;

		bitmap copy_to_image() const final;

		vec2i const & get_size() const noexcept { return m_size; }

		texture_format const & get_format() const noexcept { return m_format; }

		texture_flags_ get_flags() const noexcept { return m_flags; }
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// texture3d
namespace ml::gfx
{
	// opengl texture3d
	struct opengl_texture3d : texture3d
	{
	private:
		static constexpr typeof_t<> s_self_type{ typeof_v<opengl_texture3d> };

		vec2i			m_size		{}			; // 
		texture_format	m_format	{}			; // 
		texture_flags_	m_flags		{}			; // 
		uint32			m_handle	{}			; // 
		bool			m_locked	{ true }	; // 

	public:
		opengl_texture3d(render_device * parent, spec_type const & desc, allocator_type alloc);

		~opengl_texture3d() final;

		bool revalue() final;

		object_id get_handle() const noexcept final { return ML_handle(object_id, m_handle); }

		typeof_t<> const & get_self_type() const noexcept final { return s_self_type; }

	public:
		void lock() final;

		void unlock() final;

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
		static constexpr typeof_t<> s_self_type{ typeof_v<opengl_texturecube> };

		vec2i			m_size		{}			; // 
		texture_format	m_format	{}			; // 
		texture_flags_	m_flags		{}			; // 
		uint32			m_handle	{}			; // handle
		bool			m_locked	{ true }	; // locked

	public:
		opengl_texturecube(render_device * parent, spec_type const & desc, allocator_type alloc);

		~opengl_texturecube() final;

		bool revalue() final;

		object_id get_handle() const noexcept final { return ML_handle(object_id, m_handle); }

		typeof_t<> const & get_self_type() const noexcept final { return s_self_type; }

	public:
		void lock() final;

		void unlock() final;

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
		static constexpr typeof_t<> s_self_type{ typeof_v<opengl_framebuffer> };

		vec2i							m_size			{}; // 
		texture_format					m_format		{}; // 
		texture_flags_					m_flags			{}; // 
		vec4i							m_bpp			{}; // 
		int32							m_stencil_bits	{}; // 
		int32							m_depth_bits	{}; // 
		int32							m_samples		{}; // 
		bool							m_stereo		{}; // 
		uint32							m_handle		{}; // handle
		ds::list<ref<texture2d>>	m_attachments	{}; // color attachments
		ref<texture2d>				m_depth			{}; // depth attachment

		
	public:
		opengl_framebuffer(render_device * parent, spec_type const & desc, allocator_type alloc);

		~opengl_framebuffer() final;

		bool revalue() final;

		object_id get_handle() const noexcept final { return ML_handle(object_id, m_handle); }

		typeof_t<> const & get_self_type() const noexcept final { return s_self_type; }

	public:
		bool attach(ref<texture2d> const & value) final;

		bool detach(ref<texture2d> const & value) final;

		void resize(vec2i const & value) final;

		ds::list<ref<texture2d>> const & get_color_attachments() const noexcept final { return m_attachments; }

		ref<texture2d> const & get_depth_attachment() const noexcept final { return m_depth; }

		vec2i const & get_size() const noexcept { return m_size; }

		texture_format const & get_format() const noexcept { return m_format; }

		texture_flags_ get_flags() const noexcept { return m_flags; }

		vec4i const & get_bits_per_pixel() const noexcept final { return m_bpp; }

		int32 get_stencil_bits() const noexcept final { return m_stencil_bits; }

		int32 get_depth_bits() const noexcept final { return m_depth_bits; }

		int32 get_sample_count() const noexcept final { return m_samples; }

		bool is_stereo() const noexcept final { return m_stereo; }
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
		static constexpr typeof_t<> s_self_type{ typeof_v<opengl_program> };

		uint32									m_handle		{}; // handle
		ds::string								m_error_log		{}; // error log
		ds::array<object_id, shader_type_MAX>	m_shaders		{}; // shader cache
		ds::map<uint32, ds::list<ds::string>>	m_source		{}; // source cache
		ds::map<uniform_id, ref<texture>>	m_textures		{}; // texture cache
		ds::map<hash_t, uniform_id>				m_uniforms		{}; // uniform cache

		// uniform binder
		struct ML_NODISCARD program_uniform_binder final
		{
			uniform_id location{ ML_handle(uniform_id, -1) };

			uint32 self{}, last{};

			operator bool() const noexcept { return -1 < ML_handle(int32, location); }

			program_uniform_binder(opengl_program & s, cstring name) noexcept;

			~program_uniform_binder() noexcept;
		};

	public:
		opengl_program(render_device * parent, spec_type const & desc, allocator_type alloc);

		~opengl_program() final;

		bool revalue() final;

		object_id get_handle() const noexcept final { return ML_handle(object_id, m_handle); }

		typeof_t<> const & get_self_type() const noexcept final { return s_self_type; }

	public:
		bool attach(uint32 type, size_t count, cstring * str, int32 const * len = {}) final;

		bool detach(uint32 type) final;

		bool link() final;

		bool bind_uniform(cstring name, ds::method<void(uniform_id)> const & fn) final
		{
			program_uniform_binder u{ *this, name };
			if (u) { std::invoke(fn, u.location); }
			return u;
		}

		uniform_id get_uniform_location(cstring name) noexcept final;

		ds::string const & get_info_log() const noexcept final { return m_error_log; }

		ds::array<object_id, shader_type_MAX> const & get_shaders() const noexcept final { return m_shaders; }

		ds::map<uint32, ds::list<ds::string>> const & get_source() const noexcept final { return m_source; }

		ds::map<uniform_id, ref<texture>> const & get_textures() const noexcept final { return m_textures; }

		ds::map<hash_t, uniform_id> const & get_uniforms() const noexcept final { return m_uniforms; }

		uint32 get_mask() const noexcept final
		{
			auto const & s{ get_shaders() };
			uint32 mask{};
			ML_flag_write(mask, shader_bit_vertex	, s[shader_type_vertex]);
			ML_flag_write(mask, shader_bit_pixel	, s[shader_type_pixel]);
			ML_flag_write(mask, shader_bit_geometry	, s[shader_type_geometry]);
			return mask;
		}

	public:
		void do_cache_texture(uniform_id loc, ref<texture> const & value) noexcept final
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
		static constexpr typeof_t<> s_self_type{ typeof_v<opengl_shader> };

		uint32									m_type		{}; // type
		ds::list<ds::string>					m_code		{}; // code
		uint32									m_handle	{}; // handle
		ds::string								m_log		{}; // error log
		ds::list<ds::string>					m_source	{}; // source
		ds::map<hash_t, uniform_id>				m_attribs	{}; // attributes
		ds::map<hash_t, uniform_id>				m_uniforms	{}; // uniforms
		ds::map<uniform_id, ref<texture>>	m_textures	{}; // textures

		struct ML_NODISCARD shader_uniform_binder final
		{
			uniform_id loc{ (uniform_id)-1 };

			uint32 self{}, last{};

			operator bool() const noexcept { return -1 < ML_handle(int32, loc); }

			shader_uniform_binder(opengl_shader & s, cstring name) noexcept;

			~shader_uniform_binder() noexcept;
		};

	public:
		opengl_shader(render_device * parent, spec_type const & desc, allocator_type alloc);

		~opengl_shader() final;

		object_id get_handle() const noexcept final { return ML_handle(object_id, m_handle); }

		typeof_t<> const & get_self_type() const noexcept final { return s_self_type; }

	public:
		bool compile(uint32 type, size_t count, cstring * str, int32 const * len = {}) final;

		bool bind_uniform(cstring name, ds::method<void(uniform_id)> const & fn) final
		{
			shader_uniform_binder u{ *this, name };
			if (u) { std::invoke(fn, u.loc); }
			return u;
		}

		ds::string const & get_info_log() const noexcept final { return m_log; }

		ds::list<ds::string> const & get_source() const noexcept final { return m_source; }

		ds::map<uniform_id, ref<texture>> const & get_textures() const noexcept final { return m_textures; }

		uint32 get_type() const noexcept final { return m_type; }

		uint32 get_mask() const noexcept final
		{
			uint32 mask{};
			ML_flag_write(mask, shader_bit_vertex	, m_type == shader_type_vertex);
			ML_flag_write(mask, shader_bit_pixel	, m_type == shader_type_pixel);
			ML_flag_write(mask, shader_bit_geometry	, m_type == shader_type_geometry);
			return mask;
		}

	protected:
		void do_cache(uniform_id loc, ref<texture> const & value) final
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

		void do_upload(uniform_id loc, bool value) final;

		void do_upload(uniform_id loc, int32 value) final;

		void do_upload(uniform_id loc, uint32 value) final;

		void do_upload(uniform_id loc, float32 value) final;

		void do_upload(uniform_id loc, vec2f const & value) final;

		void do_upload(uniform_id loc, vec3f const & value) final;

		void do_upload(uniform_id loc, vec4f const & value) final;

		void do_upload(uniform_id loc, mat2f const & value, bool transpose = false) final;

		void do_upload(uniform_id loc, mat3f const & value, bool transpose = false) final;

		void do_upload(uniform_id loc, mat4f const & value, bool transpose = false) final;

		void do_upload(uniform_id loc, ref<texture> const & value, uint32 slot = 0) final;
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_IMPL_RENDERER_OPENGL_HPP_
#endif // _ML_OPENGL_RENDER_API_HPP_