#if defined(ML_IMPL_RENDERER_OPENGL)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define ML_IMPL_GFX_CHECK
#include "./OpenGL.hpp"
#include "./OpenGL_RenderAPI.hpp"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// render device
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_render_device::opengl_render_device(spec_type const & desc, allocator_type alloc) : render_device{}
	{
		static bool const opengl_init{ ML_IMPL_OPENGL_INIT() };
		ML_assert("failed initializing opengl device" && opengl_init);

		// renderer
		ML_glCheck(m_info.renderer = (cstring)glGetString(GL_RENDERER));

		// vendor
		ML_glCheck(m_info.vendor = (cstring)glGetString(GL_VENDOR));

		// version
		ML_glCheck(m_info.version = (cstring)glGetString(GL_VERSION));

		// major version
		if (glGetIntegerv(GL_MAJOR_VERSION, &m_info.major_version); glGetError() == GL_INVALID_ENUM)
		{
			m_info.major_version = !m_info.version.empty() ? m_info.version[0] - '0' : 1;
		}

		// minor version
		if (glGetIntegerv(GL_MINOR_VERSION, &m_info.minor_version); glGetError() == GL_INVALID_ENUM)
		{
			m_info.minor_version = !m_info.version.empty() ? m_info.version[2] - '0' : 1;
		}

		// extensions
		{
			int32 num{};
			ML_glCheck(glGetIntegerv(GL_NUM_EXTENSIONS, &num));
			m_info.extensions.reserve(num);

			std::stringstream ss{};
			ML_glCheck(ss.str((cstring)glGetString(GL_EXTENSIONS)));

			ds::string line{};
			while (std::getline(ss, line, ' '))
			{
				m_info.extensions.push_back(line);
			}
		}

		// texture edge clamp available
#if defined(GL_EXT_texture_edge_clamp) \
|| defined(GLEW_EXT_texture_edge_clamp) \
|| defined(GL_SGIS_texture_edge_clamp)
		m_info.texture_edge_clamp_available = true;
#endif
		// max texture slots
		ML_glCheck(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (int32 *)&m_info.max_texture_slots));

		// max color attachments
		ML_glCheck(glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, (int32 *)&m_info.max_color_attachments));

		// max samples
		ML_glCheck(glGetIntegerv(GL_MAX_SAMPLES, (int32 *)&m_info.max_samples));

		// shaders available
#if defined(GL_ARB_shading_language_100) \
|| defined(GL_ARB_shader_objects) \
|| defined(GL_ARB_vertex_shader) \
|| defined(GL_ARB_fragment_shader)
		m_info.shaders_available = true;

		// geometry shaders available
#	ifdef GL_ARB_geometry_shader4
		m_info.geometry_shaders_available = true;
#	endif

		// shading language version
		ML_glCheck(m_info.shading_language_version = (cstring)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
	}

	opengl_render_device::~opengl_render_device() {}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ds::ref<render_context> const & opengl_render_device::get_active_context() const noexcept
	{
		return m_context;
	}

	void opengl_render_device::set_active_context(ds::ref<render_context> const & value) noexcept
	{
		m_context = value;

		ML_glCheck(glBindProgramPipeline
		(
			value ? ML_handle(uint32, value->get_handle()) : NULL
		));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ds::ref<render_context> opengl_render_device::new_context(spec<render_context> const & desc, allocator_type alloc) noexcept
	{
		return m_objs.push_back<ds::unown<render_context>>(alloc_ref<opengl_render_context>(alloc, this, desc)).lock();
	}

	ds::ref<vertexarray> opengl_render_device::new_vertexarray(spec<vertexarray> const & desc, allocator_type alloc) noexcept
	{
		return m_objs.push_back<ds::unown<vertexarray>>(alloc_ref<opengl_vertexarray>(alloc, this, desc)).lock();
	}

	ds::ref<vertexbuffer> opengl_render_device::new_vertexbuffer(spec<vertexbuffer> const & desc, allocator_type alloc) noexcept
	{
		return m_objs.push_back<ds::unown<vertexbuffer>>(alloc_ref<opengl_vertexbuffer>(alloc, this, desc)).lock();
	}

	ds::ref<indexbuffer> opengl_render_device::new_indexbuffer(spec<indexbuffer> const & desc, allocator_type alloc) noexcept
	{
		return m_objs.push_back<ds::unown<indexbuffer>>(alloc_ref<opengl_indexbuffer>(alloc, this, desc)).lock();
	}

	ds::ref<texture2d> opengl_render_device::new_texture2d(spec<texture2d> const & desc, allocator_type alloc) noexcept
	{
		return m_objs.push_back<ds::unown<texture2d>>(alloc_ref<opengl_texture2d>(alloc, this, desc)).lock();
	}

	ds::ref<texture3d> opengl_render_device::new_texture3d(spec<texture3d> const & desc, allocator_type alloc) noexcept
	{
		return m_objs.push_back<ds::unown<texture3d>>(alloc_ref<opengl_texture3d>(alloc, this, desc)).lock();
	}

	ds::ref<texturecube> opengl_render_device::new_texturecube(spec<texturecube> const & desc, allocator_type alloc) noexcept
	{
		return m_objs.push_back<ds::unown<texturecube>>(alloc_ref<opengl_texturecube>(alloc, this, desc)).lock();
	}

	ds::ref<framebuffer> opengl_render_device::new_framebuffer(spec<framebuffer> const & desc, allocator_type alloc) noexcept
	{
		return m_objs.push_back<ds::unown<framebuffer>>(alloc_ref<opengl_framebuffer>(alloc, this, desc)).lock();
	}

	ds::ref<program> opengl_render_device::new_program(spec<program> const & desc, allocator_type alloc) noexcept
	{
		return m_objs.push_back<ds::unown<program>>(alloc_ref<opengl_program>(alloc, this, desc)).lock();
	}

	ds::ref<shader> opengl_render_device::new_shader(spec<shader> const & desc, allocator_type alloc) noexcept
	{
		return m_objs.push_back<ds::unown<shader>>(alloc_ref<opengl_shader>(alloc, this, desc)).lock();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// render context
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_render_context::opengl_render_context(render_device * parent, spec_type const & desc, allocator_type alloc)
		: render_context{ parent }
		, m_handle		{}
		, m_desc		{ desc }
	{
		ML_assert("invalid render context api" && desc.api == context_api_opengl);

		if (auto const major{ parent->get_info().major_version }; major != desc.major) {
			debug::warning("opengl major version mismatch: {0}!={1}", major, desc.major);
		}

		if (auto const minor{ parent->get_info().minor_version }; minor != desc.minor) {
			debug::warning("opengl minor version mismatch: {0}!={1}", minor, desc.minor);
		}

		ML_glCheck(ML_glEnable(GL_MULTISAMPLE, desc.multisample));
		ML_glCheck(ML_glEnable(GL_FRAMEBUFFER_SRGB, desc.srgb_capable));
		ML_glCheck(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

		ML_glCheck(glGenProgramPipelines(1, &m_handle));
		ML_glCheck(glBindProgramPipeline(m_handle));
		ML_glCheck(glUseProgramStages(m_handle, shader_bit_all, NULL));
	}

	opengl_render_context::~opengl_render_context()
	{
		ML_glCheck(glDeleteProgramPipelines(1, &m_handle));
	}

	bool opengl_render_context::revalue()
	{
		if (m_handle) { ML_glCheck(glDeleteProgramPipelines(1, &m_handle)); }

		ML_glCheck(glGenProgramPipelines(1, &m_handle));

		return (bool)m_handle;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	alpha_state * opengl_render_context::get_alpha_state(alpha_state * value) const
	{
		if (static alpha_state temp{}; !value) { value = &temp; }

		ML_glCheck(glGetBooleanv(GL_ALPHA_TEST, (uint8 *)&value->enabled));

		ML_glCheck(glGetIntegerv(GL_ALPHA_TEST_FUNC, (int32 *)&value->pred));
		value->pred = _predicate<to_user>(value->pred);

		ML_glCheck(glGetFloatv(GL_ALPHA_TEST_REF, &value->ref));

		return value;
	}

	blend_state * opengl_render_context::get_blend_state(blend_state * value) const
	{
		if (static blend_state temp{}; !value) { value = &temp; }

		ML_glCheck(glGetBooleanv(GL_BLEND, (uint8 *)&value->enabled));

		ML_glCheck(glGetFloatv(GL_BLEND_COLOR, value->color));

		ML_glCheck(glGetIntegerv(GL_BLEND_EQUATION_RGB, (int32 *)&value->color_equation));
		value->color_equation = _equation<to_user>(value->color_equation);

		ML_glCheck(glGetIntegerv(GL_BLEND_SRC_RGB, (int32 *)&value->color_sfactor));
		value->color_sfactor = _factor<to_user>(value->color_sfactor);

		ML_glCheck(glGetIntegerv(GL_BLEND_DST_RGB, (int32 *)&value->color_dfactor));
		value->color_dfactor = _factor<to_user>(value->color_dfactor);

		ML_glCheck(glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (int32 *)&value->alpha_equation));
		value->alpha_equation = _equation<to_user>(value->alpha_equation);

		ML_glCheck(glGetIntegerv(GL_BLEND_SRC_ALPHA, (int32 *)&value->alpha_sfactor));
		value->alpha_sfactor = _factor<to_user>(value->alpha_sfactor);

		ML_glCheck(glGetIntegerv(GL_BLEND_DST_ALPHA, (int32 *)&value->alpha_dfactor));
		value->alpha_dfactor = _factor<to_user>(value->alpha_dfactor);

		return value;
	}

	color * opengl_render_context::get_clear_color(color * value) const
	{
		if (static color temp{}; !value) { value = &temp; }

		ML_glCheck(glGetFloatv(GL_COLOR_CLEAR_VALUE, *value));

		return value;
	}

	cull_state * opengl_render_context::get_cull_state(cull_state * value) const
	{
		if (static cull_state temp{}; !value) { value = &temp; }

		ML_glCheck(glGetBooleanv(GL_CULL_FACE, (uint8 *)&value->enabled));

		ML_glCheck(glGetIntegerv(GL_CULL_FACE_MODE, (int32 *)&value->facet));
		value->facet = _facet<to_user>(value->facet);

		ML_glCheck(glGetIntegerv(GL_FRONT_FACE, (int32 *)&value->order));
		value->order = _order<to_user>(value->order);

		return value;
	}

	depth_state * opengl_render_context::get_depth_state(depth_state * value) const
	{
		if (static depth_state temp{}; !value) { value = &temp; }

		ML_glCheck(glGetBooleanv(GL_DEPTH_TEST, (uint8 *)&value->enabled));

		ML_glCheck(glGetIntegerv(GL_DEPTH_FUNC, (int32 *)&value->pred));
		value->pred = _predicate<to_user>(value->pred);
		
		ML_glCheck(glGetFloatv(GL_DEPTH_RANGE, value->range));
		
		return value;
	}

	stencil_state * opengl_render_context::get_stencil_state(stencil_state * value) const
	{
		if (static stencil_state temp{}; !value) { value = &temp; }

		ML_glCheck(glGetBooleanv(GL_STENCIL_TEST, (uint8 *)&value->enabled));
		{
			ML_glCheck(glGetIntegerv(GL_STENCIL_FUNC, (int32 *)&value->front_pred));
			value->front_pred = _predicate<to_user>(value->front_pred);

			ML_glCheck(glGetIntegerv(GL_STENCIL_REF, &value->front_ref));

			ML_glCheck(glGetIntegerv(GL_STENCIL_VALUE_MASK, (int32 *)&value->front_mask));
		}
		{
			ML_glCheck(glGetIntegerv(GL_STENCIL_BACK_FUNC, (int32 *)&value->back_pred));
			value->back_pred = _predicate<to_user>(value->back_pred);

			ML_glCheck(glGetIntegerv(GL_STENCIL_BACK_REF, &value->back_ref));

			ML_glCheck(glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, (int32 *)&value->back_mask));
		}

		return value;
	}

	int_rect * opengl_render_context::get_viewport(int_rect * value) const
	{
		if (static int_rect temp{}; !value) { value = &temp; }

		ML_glCheck(glGetIntegerv(GL_VIEWPORT, *value));

		return value;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void opengl_render_context::set_alpha_state(alpha_state const & value)
	{
		ML_glCheck(ML_glEnable(GL_ALPHA_TEST, value.enabled));

		ML_glCheck(glAlphaFunc(_predicate<to_impl>(value.pred), value.ref));
	}

	void opengl_render_context::set_blend_state(blend_state const & value)
	{
		ML_glCheck(ML_glEnable(GL_BLEND, value.enabled));
		
		ML_glCheck(glBlendColor(
			value.color[0],
			value.color[1],
			value.color[2],
			value.color[3]));
		
		ML_glCheck(glBlendFuncSeparate(
			_factor<to_impl>(value.color_sfactor),
			_factor<to_impl>(value.color_dfactor),
			_factor<to_impl>(value.alpha_sfactor),
			_factor<to_impl>(value.alpha_dfactor)));

		ML_glCheck(glBlendEquationSeparate(
			_equation<to_impl>(value.color_equation),
			_equation<to_impl>(value.alpha_equation)));
	}

	void opengl_render_context::set_clear_color(color const & value)
	{
		ML_glCheck(glClearColor(value[0], value[1], value[2], value[3]));
	}

	void opengl_render_context::set_cull_state(cull_state const & value)
	{
		ML_glCheck(ML_glEnable(GL_CULL_FACE, value.enabled));
		
		ML_glCheck(glCullFace(_facet<to_impl>(value.facet)));
		
		ML_glCheck(glFrontFace(_order<to_impl>(value.order)));
	}

	void opengl_render_context::set_depth_state(depth_state const & value)
	{
		ML_glCheck(ML_glEnable(GL_DEPTH_TEST, value.enabled));
		
		ML_glCheck(glDepthFunc(_predicate<to_impl>(value.pred)));

		ML_glCheck(glDepthRangef(value.range[0], value.range[1]));
	}

	void opengl_render_context::set_stencil_state(stencil_state const & value)
	{
		ML_glCheck(ML_glEnable(GL_STENCIL_TEST, value.enabled));

		ML_glCheck(glStencilFuncSeparate(
			GL_FRONT,
			_predicate<to_impl>(value.front_pred),
			value.front_ref,
			value.front_mask));
		
		ML_glCheck(glStencilFuncSeparate(
			GL_BACK,
			_predicate<to_impl>(value.back_pred),
			value.back_ref,
			value.back_mask));
	}

	void opengl_render_context::set_viewport(int_rect const & value)
	{
		ML_glCheck(glViewport(value[0], value[1], value[2], value[3]));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void opengl_render_context::clear(uint32 mask)
	{
		uint32 temp{};
		ML_flag_map(temp, GL_COLOR_BUFFER_BIT	, mask, clear_flags_color		);
		ML_flag_map(temp, GL_DEPTH_BUFFER_BIT	, mask, clear_flags_depth		);
		ML_flag_map(temp, GL_STENCIL_BUFFER_BIT	, mask, clear_flags_stencil	);
		ML_glCheck(glClear(temp));
	}

	void opengl_render_context::draw(ds::ref<vertexarray> const & value)
	{
		// could be moved into header file

		if (!value || value->get_vertices().empty()) { return; }

		bind_vertexarray(value.get());

		if (auto const prim{ value->get_mode() }
		; auto const & ib{ value->get_indices() })
		{
			bind_indexbuffer(ib.get());

			for (auto const & vb : value->get_vertices())
			{
				bind_vertexbuffer(vb.get());

				draw_indexed(prim, ib->get_count());
			}
		}
		else
		{
			for (auto const & vb : value->get_vertices())
			{
				bind_vertexbuffer(vb.get());

				draw_arrays(prim, 0, vb->get_count());
			}
		}
	}

	void opengl_render_context::draw_arrays(uint32 prim, size_t first, size_t count)
	{
		ML_glCheck(glDrawArrays(_primitive<to_impl>(prim), (uint32)first, (uint32)count));
	}

	void opengl_render_context::draw_indexed(uint32 prim, size_t count)
	{
		ML_glCheck(glDrawElements(_primitive<to_impl>(prim), (uint32)count, GL_UNSIGNED_INT, nullptr));
	}

	void opengl_render_context::flush()
	{
		ML_glCheck(glFlush());
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void opengl_render_context::bind_vertexarray(vertexarray const * value)
	{
		ML_glCheck(glBindVertexArray(value ? ML_handle(uint32, value->get_handle()) : NULL));
	}

	void opengl_render_context::bind_vertexbuffer(vertexbuffer const * value)
	{
		ML_glCheck(glBindBuffer(GL_ARRAY_BUFFER, value ? ML_handle(uint32, value->get_handle()) : NULL));
	}

	void opengl_render_context::bind_indexbuffer(indexbuffer const * value)
	{
		ML_glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, value ? ML_handle(uint32, value->get_handle()) : NULL));
	}

	void opengl_render_context::bind_texture(texture const * value, uint32 slot)
	{
		ML_glCheck(glBindTextureUnit(slot, value ? ML_handle(uint32, value->get_handle()) : NULL));
	}

	void opengl_render_context::bind_framebuffer(framebuffer const * value)
	{
		if (value)
		{
			ML_glCheck(glBindFramebuffer(GL_FRAMEBUFFER, ML_handle(uint32, value->get_handle())));

			set_viewport({ vec2i::zero(), value->get_size() });
		}
		else
		{
			ML_glCheck(glBindFramebuffer(GL_FRAMEBUFFER, NULL));
		}
	}

	void opengl_render_context::bind_program(program const * value)
	{
		ML_glCheck(ML_glUseProgram(value ? ML_handle(uint32, value->get_handle()) : NULL));
	}

	void opengl_render_context::bind_shader(shader const * value)
	{
		if (value)
		{
			ML_glCheck(glUseProgramStages(m_handle, value->get_mask(), ML_handle(uint32, value->get_handle())));
		}
		else
		{
			ML_glCheck(glUseProgramStages(m_handle, shader_bit_all, NULL));
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	void opengl_render_context::upload(uniform_id loc, bool value)
	{
		ML_glCheck(ML_glUniform1i(ML_handle(int32, loc), (int32)value));
	}

	void opengl_render_context::upload(uniform_id loc, int32 value)
	{
		ML_glCheck(ML_glUniform1i(ML_handle(int32, loc), value));
	}

	void opengl_render_context::upload(uniform_id loc, float32 value)
	{
		ML_glCheck(ML_glUniform1f(ML_handle(int32, loc), value));
	}

	void opengl_render_context::upload(uniform_id loc, vec2f const & value)
	{
		ML_glCheck(ML_glUniform2f(ML_handle(int32, loc), value[0], value[1]));
	}

	void opengl_render_context::upload(uniform_id loc, vec3f const & value)
	{
		ML_glCheck(ML_glUniform3f(ML_handle(int32, loc), value[0], value[1], value[2]));
	}

	void opengl_render_context::upload(uniform_id loc, vec4f const & value)
	{
		ML_glCheck(ML_glUniform4f(ML_handle(int32, loc), value[0], value[1], value[2], value[3]));
	}

	void opengl_render_context::upload(uniform_id loc, mat2f const & value)
	{
		ML_glCheck(ML_glUniformMatrix2fv(ML_handle(int32, loc), 1, false, value));
	}

	void opengl_render_context::upload(uniform_id loc, mat3f const & value)
	{
		ML_glCheck(ML_glUniformMatrix3fv(ML_handle(int32, loc), 1, false, value));
	}

	void opengl_render_context::upload(uniform_id loc, mat4f const & value)
	{
		ML_glCheck(ML_glUniformMatrix4fv(ML_handle(int32, loc), 1, false, value));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// vertexarray
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_vertexarray::opengl_vertexarray(render_device * parent, spec_type const & desc, allocator_type alloc)
		: vertexarray{ parent }
		, m_mode{ desc.prim }
	{
		ML_glCheck(glGenVertexArrays(1, &m_handle));
		ML_glCheck(glBindVertexArray(m_handle));
	}

	opengl_vertexarray::~opengl_vertexarray()
	{
		ML_glCheck(glDeleteVertexArrays(1, &m_handle));
	}

	bool opengl_vertexarray::revalue()
	{
		if (m_handle) { ML_glCheck(glDeleteVertexArrays(1, &m_handle)); }

		m_vertices.clear(); m_indices.reset();
		
		ML_glCheck(glGenVertexArrays(1, &m_handle));

		return (bool)m_handle;
	}

	void opengl_vertexarray::add_vertices(ds::ref<vertexbuffer> const & value)
	{
		if (!m_handle || !value) { return; }
		
		bind();
		
		m_vertices.emplace_back(value)->bind();

		for (size_t i = 0; i < m_layout.elements().size(); ++i)
		{
			auto const & e{ m_layout.elements()[i] };

			if (auto const type{ std::invoke([&]() noexcept -> uint32
			{
				switch (e.get_base_type())
				{
				default					: return 0			; // unknown
				case hashof_v<bool>		: return GL_BOOL	; // bool
				case hashof_v<int32>	: return GL_INT		; // int
				case hashof_v<float32>	: return GL_FLOAT	; // float
				}
			}) }; type == GL_INT)
			{
				ML_glCheck(glVertexAttribIPointer(
					(uint32)i,
					e.get_component_count(),
					type,
					m_layout.stride(),
					reinterpret_cast<addr_t>(e.offset)));
			}
			else
			{
				ML_glCheck(glVertexAttribPointer(
					(uint32)i,
					e.get_component_count(),
					type,
					e.normalized,
					m_layout.stride(),
					reinterpret_cast<addr_t>(e.offset)));
			}
			ML_glCheck(glEnableVertexAttribArray((uint32)i));
		}
	}

	void opengl_vertexarray::set_indices(ds::ref<indexbuffer> const & value)
	{
		bind();

		if (m_indices = value) { m_indices->bind(); }
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// vertexbuffer
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_vertexbuffer::opengl_vertexbuffer(render_device * parent, spec_type const & desc, allocator_type alloc)
		: vertexbuffer{ parent }
		, m_usage{ desc.usage }
		, m_buffer{ bufcpy<float32>(desc.count, desc.data), alloc }
	{
		ML_glCheck(glGenBuffers(1, &m_handle));
		ML_glCheck(glBindBuffer(GL_ARRAY_BUFFER, m_handle));
		ML_glCheck(glBufferData(
			GL_ARRAY_BUFFER,
			(uint32)m_buffer.size(),
			m_buffer.data(),
			_usage<to_impl>(m_usage)));
	}

	opengl_vertexbuffer::~opengl_vertexbuffer()
	{
		ML_glCheck(glDeleteBuffers(1, &m_handle));
	}

	bool opengl_vertexbuffer::revalue()
	{
		if (m_handle) { ML_glCheck(glDeleteBuffers(1, &m_handle)); }

		m_buffer.clear();

		ML_glCheck(glGenBuffers(1, &m_handle));

		return (bool)m_handle;
	}

	void opengl_vertexbuffer::set_data(size_t count, addr_t data, size_t offset)
	{
		m_buffer = bufcpy<float32>(count, data);
		ML_glCheck(glBufferSubData(
			GL_ARRAY_BUFFER,
			offset,
			(uint32)m_buffer.size(),
			m_buffer.data()));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// indexbuffer
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_indexbuffer::opengl_indexbuffer(render_device * parent, spec_type const & desc, allocator_type alloc)
		: indexbuffer	{ parent }
		, m_usage		{ desc.usage }
		, m_buffer		{ bufcpy<uint32>(desc.count, desc.data), alloc }
	{
		ML_glCheck(glGenBuffers(1, &m_handle));
		ML_glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle));
		ML_glCheck(glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			(uint32)m_buffer.size(),
			m_buffer.data(),
			_usage<to_impl>(m_usage)));
	}

	opengl_indexbuffer::~opengl_indexbuffer()
	{
		ML_glCheck(glDeleteBuffers(1, &m_handle));
	}

	bool opengl_indexbuffer::revalue()
	{
		if (m_handle) { ML_glCheck(glDeleteBuffers(1, &m_handle)); }

		m_buffer.clear();
		
		ML_glCheck(glGenBuffers(1, &m_handle));
		
		return (bool)m_handle;
	}

	void opengl_indexbuffer::set_data(size_t count, addr_t data, size_t offset)
	{
		m_buffer = bufcpy<uint32>(count, data);
		ML_glCheck(glBufferSubData(
			GL_ELEMENT_ARRAY_BUFFER,
			offset,
			(uint32)m_buffer.size(),
			m_buffer.data()));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// texture2d
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_texture2d::opengl_texture2d(render_device * parent, spec_type const & desc, allocator_type alloc)
		: texture2d	{ parent }
		, m_size	{ desc.size }
		, m_format	{ desc.format }
		, m_flags	{ desc.flags }
	{
		ML_glCheck(glGenTextures(1, &m_handle));
		ML_glCheck(glBindTexture(GL_TEXTURE_2D, m_handle));
		ML_glCheck(glTexImage2D(
			GL_TEXTURE_2D,
			0,
			_format<to_impl>(m_format.color),
			m_size[0],
			m_size[1],
			0,
			_format<to_impl>(m_format.pixel),
			_type<to_impl>(m_format.type),
			desc.data));
		set_repeated(m_flags & texture_flags_repeat);
		set_smooth(m_flags & texture_flags_smooth);
		set_mipmapped(m_flags & texture_flags_mipmap);
	}

	opengl_texture2d::~opengl_texture2d()
	{
		ML_glCheck(glDeleteTextures(1, &m_handle));
	}

	bool opengl_texture2d::revalue()
	{
		if (!m_locked) { return debug::failure("texture2d is not locked"); }

		if (m_handle) { ML_glCheck(glDeleteTextures(1, &m_handle)); }
		
		ML_glCheck(glGenTextures(1, &m_handle));

		return (bool)m_handle;
	}

	void opengl_texture2d::lock()
	{
		m_locked = true;

		debug::warning("texture lock/unlock NYI");
	}

	void opengl_texture2d::unlock()
	{
		m_locked = false;

		debug::warning("texture lock/unlock NYI");
	}

	void opengl_texture2d::update(vec2i const & size, addr_t data)
	{
		if (!m_locked) { return (void)debug::failure("texture2d is not locked"); }

		if (m_handle && (m_size == size)) { return; }
		else { m_size = size; }

		revalue(); bind();
		
		ML_glCheck(glTexImage2D(
			GL_TEXTURE_2D,
			0,
			_format<to_impl>(m_format.color),
			m_size[0],
			m_size[1],
			0,
			_format<to_impl>(m_format.pixel),
			_type<to_impl>(m_format.type),
			data));
		
		set_repeated(m_flags & texture_flags_repeat);
		set_smooth(m_flags & texture_flags_smooth);
		set_mipmapped(m_flags & texture_flags_mipmap);
	}

	void opengl_texture2d::update(vec2i const & pos, vec2i const & size, addr_t data)
	{
		if (!m_locked) { return (void)debug::failure("texture2d is not locked"); }

		if (m_handle && (m_size == size)) { return; }
		else { m_size = size; }

		revalue(); bind();

		ML_glCheck(glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			pos[0],
			pos[1],
			m_size[0],
			m_size[1],
			_format<to_impl>(m_format.color),
			_type<to_impl>(m_format.type),
			data));
		
		set_repeated(m_flags & texture_flags_repeat);
		set_smooth(m_flags & texture_flags_smooth);
		set_mipmapped(m_flags & texture_flags_mipmap);
	}

	void opengl_texture2d::set_mipmapped(bool value)
	{
		if (!m_locked) { return (void)debug::failure("texture2d is not locked"); }

		ML_flag_write(m_flags, texture_flags_mipmap, value);

		bool const smooth{ ML_flag_read(m_flags, texture_flags_smooth) };

		if (smooth) { ML_glCheck(glGenerateMipmap(GL_TEXTURE_2D)); }

		ML_glCheck(glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_MAG_FILTER,
			value
			? smooth ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR
			: smooth ? GL_LINEAR : GL_NEAREST));
	}

	void opengl_texture2d::set_repeated(bool value)
	{
		if (!m_locked) { return (void)debug::failure("texture2d is not locked"); }

		ML_flag_write(m_flags, texture_flags_repeat, value);

		static bool const edge_clamp_available
		{
			get_device()->get_info().texture_edge_clamp_available
		};

		ML_glCheck(glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_S,
			value
			? GL_REPEAT
			: edge_clamp_available ? GL_CLAMP_TO_EDGE : GL_CLAMP));

		ML_glCheck(glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_T,
			value
			? GL_REPEAT
			: edge_clamp_available ? GL_CLAMP_TO_EDGE : GL_CLAMP));
	}

	void opengl_texture2d::set_smooth(bool value)
	{
		if (!m_locked) { return (void)debug::failure("texture2d is not locked"); }

		ML_flag_write(m_flags, texture_flags_smooth, value);

		ML_glCheck(glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_MAG_FILTER,
			value ? GL_LINEAR : GL_NEAREST));

		ML_glCheck(glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_MIN_FILTER,
			ML_flag_read(m_flags, texture_flags_mipmap)
			? value ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR
			: value ? GL_LINEAR : GL_NEAREST));
	}

	bitmap opengl_texture2d::copy_to_image() const
	{
		if (!m_locked) { debug::failure("texture2d is not locked"); return bitmap{}; }

		bitmap temp{ m_size, calc_bits_per_pixel(m_format.color) };
		if (m_handle)
		{
			bind();

			ML_glCheck(glGetTexImage(GL_TEXTURE_2D, 0,
				_format<to_impl>(m_format.color),
				_type<to_impl>(m_format.type),
				temp.data()));

			unbind();
		}
		return temp;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// texture3d
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_texture3d::opengl_texture3d(render_device * parent, spec_type const & desc, allocator_type alloc)
		: texture3d	{ parent }
		, m_size	{ desc.size }
		, m_format	{ desc.format }
		, m_flags	{ desc.flags }
	{
		ML_glCheck(glGenTextures(1, &m_handle));
	}

	opengl_texture3d::~opengl_texture3d()
	{
		ML_glCheck(glDeleteTextures(1, &m_handle));
	}

	bool opengl_texture3d::revalue()
	{
		if (!m_locked) { return debug::failure("texture3d is not locked"); }

		if (m_handle) { ML_glCheck(glDeleteTextures(1, &m_handle)); }
		
		ML_glCheck(glGenTextures(1, &m_handle));
		
		return (bool)m_handle;
	}

	void opengl_texture3d::lock()
	{
		m_locked = true;

		debug::warning("texture lock/unlock NYI");
	}

	void opengl_texture3d::unlock()
	{
		m_locked = false;

		debug::warning("texture lock/unlock NYI");
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// texturecube
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_texturecube::opengl_texturecube(render_device * parent, spec_type const & desc, allocator_type alloc)
		: texturecube	{ parent }
		, m_size		{ desc.size }
		, m_format		{ desc.format }
		, m_flags		{ desc.flags }
	{
		ML_glCheck(glGenTextures(1, &m_handle));
	}

	opengl_texturecube::~opengl_texturecube()
	{
		ML_glCheck(glDeleteTextures(1, &m_handle));
	}

	bool opengl_texturecube::revalue()
	{
		if (!m_locked) { return debug::failure("texturecube is not locked"); }

		if (m_handle) { ML_glCheck(glDeleteTextures(1, &m_handle)); }
		
		ML_glCheck(glGenTextures(1, &m_handle));
		
		return (bool)m_handle;
	}

	void opengl_texturecube::lock()
	{
		m_locked = true;

		debug::warning("texture lock/unlock NYI");
	}

	void opengl_texturecube::unlock()
	{
		m_locked = false;

		debug::warning("texture lock/unlock NYI");
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// framebuffer
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_framebuffer::opengl_framebuffer(render_device * parent, spec_type const & desc, allocator_type alloc)
		: framebuffer	{ parent }
		, m_size		{ desc.size }
		, m_format		{ desc.format }
		, m_flags		{ desc.flags }
		, m_bpp			{ desc.bits_per_pixel }
		, m_stencil_bits{ desc.stencil_bits }
		, m_depth_bits	{ desc.depth_bits }
		, m_samples		{ desc.samples }
		, m_stereo		{ desc.stereo }
	{
		resize(m_size);
	}

	opengl_framebuffer::~opengl_framebuffer()
	{
		ML_glCheck(glDeleteFramebuffers(1, &m_handle));
	}

	bool opengl_framebuffer::revalue()
	{
		if (m_handle) { ML_glCheck(glDeleteFramebuffers(1, &m_handle)); }
		
		ML_glCheck(glGenFramebuffers(1, &m_handle));
		
		m_attachments.clear(); m_depth.reset();

		return (bool)m_handle;
	}

	bool opengl_framebuffer::attach(ds::ref<texture2d> const & value)
	{
		static auto const max_color_attachments
		{
			(size_t)get_device()->get_info().max_color_attachments
		};
		
		if (m_attachments.size() < max_color_attachments &&
			!std::binary_search(m_attachments.begin(), m_attachments.end(), value)
		)
		{
			m_attachments.push_back(value);
			return true;
		}
		return false;
	}

	bool opengl_framebuffer::detach(ds::ref<texture2d> const & value)
	{
		if (auto const it{ std::find(m_attachments.begin(), m_attachments.end(), value) }
		; it != m_attachments.end())
		{
			m_attachments.erase(it);

			return true;
		}
		return false;
	}

	void opengl_framebuffer::resize(vec2i const & size)
	{
		if (m_handle && (m_size == size)) { return; }
		else { m_size = size; }

		revalue(); bind();

		// color attachments
		if (m_attachments.empty())
		{
			m_attachments.push_back(get_device()->new_texture2d({
				m_size,
				m_format,
				m_flags
				}));
		}
		for (size_t i = 0, imax = m_attachments.size(); i < imax; ++i)
		{
			m_attachments[i]->update(m_size);

			ML_glCheck(glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				GL_COLOR_ATTACHMENT0 + (uint32)i,
				GL_TEXTURE_2D,
				ML_handle(uint32, m_attachments[i]->get_handle()),
				0));
		}

		// depth attachment
		if (m_depth)
		{
			m_depth->update(m_size);
		}
		else
		{
			m_depth = get_device()->new_texture2d({
				m_size,
				{ format_depth24_stencil8, format_depth_stencil, type_uint_24_8 },
				m_flags
				});
		}
		ML_glCheck(glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_DEPTH_STENCIL_ATTACHMENT,
			GL_TEXTURE_2D,
			ML_handle(uint32, m_depth->get_handle()),
			0));

		// check status
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			debug::failure("framebuffer is not complete");
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// program
namespace ml::gfx
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_program::program_uniform_binder::program_uniform_binder(opengl_program & p, cstring name) noexcept
	{
		if (!name || !*name || !(self = p.m_handle)) { return; }
		
		ML_glCheck(last = ML_glGetProgram());

		if (self != last) { ML_glCheck(ML_glUseProgram(self)); }

		location = p.m_uniforms.find_or_add_fn(hashof(name, std::strlen(name)), [&
		]() noexcept
		{
			int32 temp{};
			ML_glCheck(temp = ML_glGetUniformLocation(self, name));
			return ML_handle(uniform_id, temp);
		});
	}

	opengl_program::program_uniform_binder::~program_uniform_binder() noexcept
	{
		if (self && (self != last))
		{
			ML_glCheck(glUseProgramObjectARB(last));
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	opengl_program::opengl_program(render_device * parent, spec_type const & desc, allocator_type alloc)
		: program{ parent }
	{
		ML_glCheck(m_handle = ML_glCreateProgram());
	}

	opengl_program::~opengl_program()
	{
		ML_glCheck(ML_glDeleteProgram(m_handle));
	}

	bool opengl_program::revalue()
	{
		if (m_handle) { ML_glCheck(ML_glDeleteProgram(m_handle)); }

		m_uniforms.clear(); m_textures.clear(); m_shaders.clear();
		
		ML_glCheck(m_handle = ML_glCreateProgram());
		
		return (bool)m_handle;
	}

	bool opengl_program::attach(uint32 type, size_t count, cstring * str, int32 const * len)
	{
		if (!count || !str || !*str || m_shaders.contains(type)) { return false; }

		// create shader
		uint32 temp{};
		ML_glCheck(temp = ML_glCreateShader(_shader_type<to_impl>(type)));

		// compile shader
		ML_glCheck(ML_glShaderSource(temp, (uint32)count, str, len));
		ML_glCheck(ML_glCompileShader(temp));

		// check compile errors
		int32 success{};
		ML_glCheck(ML_glGetShaderCompileStatus(temp, &success));
		if (!success)
		{
			gl_get_program_info_log(m_handle, m_error_log);
			
			ML_glCheck(ML_glDeleteShader(temp));
		}
		else
		{
			// attach shader
			ML_glCheck(ML_glAttachShader(m_handle, temp));
			m_shaders.insert(type, ML_handle(object_id, temp));
			m_source[type] = { str, str + count };
		}
		return success;
	}

	bool opengl_program::detach(uint32 type)
	{
		if (auto const it{ m_shaders.find(type) })
		{
			// detach shader
			ML_glCheck(ML_glDetachShader(m_handle, ML_handle(uint32, *it->second)));

			// delete shader
			ML_glCheck(ML_glDeleteShader(ML_handle(uint32, *it->second)));

			m_shaders.erase(it->first);

			m_source[type].clear();

			return true;
		}
		return false;
	}

	bool opengl_program::link()
	{
		// link
		ML_glCheck(ML_glLinkProgram(m_handle));

		// check linker errors
		int32 success{};
		ML_glCheck(ML_glGetProgramLinkStatus(m_handle, &success));
		if (!success)
		{
			gl_get_program_info_log(m_handle, m_error_log);
		}
		return success;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// shader
namespace ml::gfx
{
	opengl_shader::shader_uniform_binder::shader_uniform_binder(opengl_shader & s, cstring name) noexcept
	{
		if (!name || !*name || !(self = s.m_handle)) { return; }

		ML_glCheck(last = ML_glGetProgram());

		if (self != last) { ML_glCheck(ML_glUseProgram(self)); }

		loc = s.m_uniforms.find_or_add_fn(hashof(name, std::strlen(name)), [&
		]() noexcept
		{
			int32 temp{};
			ML_glCheck(temp = ML_glGetUniformLocation(self, name));
			return ML_handle(uniform_id, temp);
		});
	}

	opengl_shader::shader_uniform_binder::~shader_uniform_binder() noexcept
	{
		if (self && (self != last))
		{
			ML_glCheck(ML_glUseProgram(last));
		}
	}

	opengl_shader::opengl_shader(render_device * parent, spec_type const & desc, allocator_type alloc)
		: shader{ parent }
	{
		cstring str{ desc.code.front().data() };
		compile(desc.type, desc.code.size(), &str);
	}

	opengl_shader::~opengl_shader()
	{
		ML_glCheck(ML_glDeleteProgram(m_handle));
	}

	bool opengl_shader::compile(uint32 type, size_t count, cstring * str, int32 const * len)
	{
		if (!count || !str || !*str) { return false; }

		if (m_handle)
		{
			ML_glCheck(glDeleteProgram(m_handle));
			m_uniforms.clear(); m_textures.clear();
		}

		m_type = type;
		m_code = { str, str + count };

		uint32 temp{};
		ML_glCheck(temp = glCreateShader(_shader_type<to_impl>(type)));
		if (temp)
		{
			ML_glCheck(glShaderSource(temp, (uint32)count, str, len));
			ML_glCheck(glCompileShader(temp));

			ML_glCheck(m_handle = glCreateProgram());
			if (m_handle)
			{
				int32 compiled{};
				ML_glCheck(glGetShaderiv(temp, GL_COMPILE_STATUS, &compiled));
				ML_glCheck(glProgramParameteri(m_handle, GL_PROGRAM_SEPARABLE, true));
				if (compiled)
				{
					ML_glCheck(glAttachShader(m_handle, temp));
					ML_glCheck(glLinkProgram(m_handle));
					ML_glCheck(glDetachShader(m_handle, temp));
				}
				else
				{
					gl_get_program_info_log(temp, m_log);
				}
			}
			ML_glCheck(glDeleteShader(temp));
			return m_handle;
		}
		else
		{
			return false;
		}
	}

	void opengl_shader::do_upload(uniform_id loc, bool value)
	{
		ML_glCheck(glProgramUniform1i(m_handle, ML_handle(int32, loc), (int32)value));
	}

	void opengl_shader::do_upload(uniform_id loc, int32 value)
	{
		ML_glCheck(glProgramUniform1i(m_handle, ML_handle(int32, loc), value));
	}

	void opengl_shader::do_upload(uniform_id loc, uint32 value)
	{
		ML_glCheck(glProgramUniform1i(m_handle, ML_handle(int32, loc), (int32)value));
	}

	void opengl_shader::do_upload(uniform_id loc, float32 value)
	{
		ML_glCheck(glProgramUniform1f(m_handle, ML_handle(int32, loc), value));
	}

	void opengl_shader::do_upload(uniform_id loc, vec2f const & value)
	{
		ML_glCheck(glProgramUniform2f(m_handle, ML_handle(int32, loc), value[0], value[1]));
	}

	void opengl_shader::do_upload(uniform_id loc, vec3f const & value)
	{
		ML_glCheck(glProgramUniform3f(m_handle, ML_handle(int32, loc), value[0], value[1], value[2]));
	}

	void opengl_shader::do_upload(uniform_id loc, vec4f const & value)
	{
		ML_glCheck(glProgramUniform4f(m_handle, ML_handle(int32, loc), value[0], value[1], value[2], value[3]));
	}

	void opengl_shader::do_upload(uniform_id loc, mat2f const & value, bool transpose)
	{
		ML_glCheck(glProgramUniformMatrix2fv(m_handle, ML_handle(int32, loc), 1, transpose, value));
	}

	void opengl_shader::do_upload(uniform_id loc, mat3f const & value, bool transpose)
	{
		ML_glCheck(glProgramUniformMatrix3fv(m_handle, ML_handle(int32, loc), 1, transpose, value));
	}

	void opengl_shader::do_upload(uniform_id loc, mat4f const & value, bool transpose)
	{
		ML_glCheck(glProgramUniformMatrix4fv(m_handle, ML_handle(int32, loc), 1, transpose, value));
	}
	
	void opengl_shader::do_upload(uniform_id loc, ds::ref<texture> const & value, uint32 slot)
	{
		get_context()->bind_texture(value.get(), slot);

		ML_glCheck(glProgramUniform1i(m_handle, ML_handle(int32, loc), (int32)slot));
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // ML_IMPL_RENDERER_OPENGL