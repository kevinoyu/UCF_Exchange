#define MEMORY_DEFS    \
  using __ptr = ptr_t; \
  using size_t__ = typename std::underlying_type<ptr_t>::type;

template <class T, typename ptr_t, size_t SIZE_HINT>
class Pool
{
public:
	MEMORY_DEFS;
	std::vector<T> m_allocated;
	std::vector<ptr_t> m_free;
	pool() { m_allocated.reserve(SIZE_HINT); }
	pool(size_t reserve_size) { m_allocated.reserve(reserve_size); }
	T *get(ptr_t idx) { return &m_allocated[size_t__(idx)]; }
	T &operator[](ptr_t idx) { return m_allocated[size_t__(idx)]; }
#define ALLOC_INVARIANT \
  (m_free_size >= 0) /* aka can't free more than has been allocated */
	__ptr alloc(void)
	{
		if (m_free.empty()) {
			auto ret = __ptr(m_allocated.size());
			m_allocated.push_back(T());
			return ret;
		}
		else {
			auto ret = __ptr(m_free.back());
			m_free.pop_back();
			return ret;
		}
	}
	void free(__ptr idx) { m_free.push_back(idx); }
#undef ALLOC_INVARIANT
};