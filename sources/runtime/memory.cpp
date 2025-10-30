#include <covscript/runtime/memory.hpp>

std::size_t cs::memory_manager::gc(bool force)
{
	// When not reach threshold, return
	if (m_heap.size() == m_last_heap_size || (m_heap.size() < m_last_heap_size + m_gc_threshold && !force))
		return 0;
	// Clear reachable flag
	for (auto &ptr : m_heap)
		ptr.reachable_count = 0;
	// Traversal stack mark reachable
	for (auto &val : m_stack)
		val.gc_mark_reachable();
	// Delete unreachable objects
	std::size_t prev_size = m_heap.size();
	for (auto it = m_heap.begin(); it != m_heap.end();)
	{
		if (it->reachable_count == 0)
			it = m_heap.erase(it);
		else
			++it;
	}
	m_last_heap_size = m_heap.size();
	return prev_size - m_last_heap_size;
}