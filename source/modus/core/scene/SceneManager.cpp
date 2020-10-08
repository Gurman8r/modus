#include <core/scene/SceneManager.hpp>
#include <core/scene/Components.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	scene_manager::scene_manager(allocator_type alloc) noexcept : m_scenes{ alloc }
	{
	}

	scene_manager::~scene_manager() noexcept
	{
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	shared<scene> & scene_manager::create_scene(allocator_type alloc) noexcept
	{
		auto & temp{ m_scenes.emplace_back(memory::alloc_ref<scene>(alloc, this)) };
		// ...
		return temp;
	}

	void scene_manager::destroy_scene(shared<scene> const & value) noexcept
	{
		if (auto const it{ std::find(m_scenes.begin(), m_scenes.end(), value) }
		; it != m_scenes.end()) { m_scenes.erase(it); }
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}