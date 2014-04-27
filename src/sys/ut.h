#ifndef UT_H
#define UT_H

#define NOCOPY(type) private: type(const type&); void operator=(const type&);

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

const float DT = 1.0f / 60.0f;
const float PI = 3.14159265f;
const float PI_2 = 2.0f * PI;

struct placement_new { };

inline void* operator new(size_t, const placement_new&, void* where) { return where; }
inline void operator delete(void*, const placement_new&, void*) { }

template<typename T> T* construct(T* p)											{ return new(placement_new(), (void*)p) T(); }
template<typename T, typename Src> T* copy_construct(T* p, const Src& src)		{ return new(placement_new(), p) T(src); }
template<typename T> void destruct(T* p)										{ p->~T(); }
template<typename T> void destruct_range(T* first, T* last)						{ for(; first != last; ++first) destruct(first); }

template<typename T> T min(const T& a, const T& b)								{ return a < b ? a : b; }
template<typename T> T max(const T& a, const T& b)								{ return a < b ? b : a; }
template<typename T> T clamp(const T& v, const T& low, const T& high)			{ return (v < low) ? low : ((v > high) ? high : v); }
template<typename T> void swap(T& a, T& b)										{ T t = b; b = a; a = t; }
template<typename T> T lerp(const T& a, const T& b, float t)					{ return a + (b - a) * t; }

inline float square(float v)													{ return v * v; }
inline float square_signed(float v)												{ return v * fabs(v); }
inline int fast_floor(float f)													{ int i = (int)f; return f < i ? i - 1 : i; }
inline int fast_ceil(float f)													{ int i = (int)f; return f > i ? i + 1 : i; }

template<typename T>
struct array {
	array() : _elems(), _size(), _cap() { }
	~array() { free(); }

	bool reserve(u32 min_cap) {
		if (min_cap <= _cap) return true;

		T* elems = (T*)malloc(sizeof(T) * min_cap);

		if (!elems) return false;

		memcpy(elems, _elems, sizeof(T) * _size);
		::free((void*)_elems);

		_elems	= elems;
		_cap	= min_cap;

		return true;
	}

	bool ensure_available(u32 min_available) {
		if (min_available <= _cap - _size) return true;
		return reserve(max(_size + min_available, max(_cap * 2, 64 / sizeof(T))));
	}

	void free() {
		destruct_range(_elems, _elems + _size);
		::free(_elems);
		abandon();
	}

	void abandon() {
		_elems = 0; _size = 0; _cap = 0;
	}

	T* push_back(const T& v) {
		if (!ensure_available(1)) return false;
		return copy_construct(_elems + _size++, v);
	}

	void pop_back() {
		if (_size > 0) destruct(_elems + (--_size));
	}

	T* insert(u32 i, const T& v) {
		if (i <= _size) {
			if (ensure_available(1)) {
				T* p = _elems + i;
				memmove(p + 1, p, sizeof(T) * (_size - i));
				_size++;
				return copy_construct(p, v);
			}
		}
		return 0;
	}

	void erase(u32 i) {
		if (i < _size) {
			destruct(_elems + i);
			memmove(_elems + i, _elems + i + 1, sizeof(T) * (_size - 1));
			_size--;
		}
	}

	void swap_erase(u32 i) {
		if (i < _size) {
			destruct(_elems + i);
			_size--;
			if (i != _size) memcpy(_elems + i, _elems + _size, sizeof(T));
		}
	}

	bool empty() const { return _size == 0; }
	bool not_empty() const { return _size != 0; }

	u32 size() const { return _size; }
	u32 capacity() const { return _cap; }

	T* at(u32 i) { return (i >= 0 && i < _size) ? _elems + i : 0; }
	T& operator[](u32 i) { assert(i >= 0 && i < _size); return _elems[i]; }

	T& front() { assert(_size > 0); return _elems[0]; }
	T& back() { assert(_size > 0); return _elems[_size - 1]; }

	T* begin() { return _elems; }
	T* end() { return _elems + _size; }

	T* _elems;
	u32 _size;
	u32 _cap;

	NOCOPY(array<T>);
};

template<typename T>
struct list {
	list() { }
	~list() { free(); }

	bool reserve(u32 min_cap) { return _arr.reserve(min_cap); }
	bool ensure_available(u32 min_available) { return _arr.ensure_available(min_available); }

	void free() {
		for(T* p : _arr) delete p;
		_arr.free();
	}

	T* push_back(T* p) {
		if (p && _arr.push_back(p)) return p;
		delete p;
		return 0;
	}

	void pop_back() {
		if (_arr.not_empty()) {
			delete _arr.back();
			_arr.pop_back();
		}
	}

	T* insert(u32 i, T* p) {
		if (p && _arr.insert(i, p)) return p;
		delete p;
		return 0;
	}

	void erase(u32 i) {
		if (i < _arr.size()) {
			delete _arr[i];
			_arr.erase(i);
		}
	}

	void swap_erase(u32 i) {
		if (i < _arr.size()) {
			delete _arr[i];
			_arr.swap_erase(i);
		}
	}

	bool empty() const { return _arr.empty(); }
	bool not_empty() const { return _arr.not_empty(); }

	u32 size() const { return _arr.size(); }
	u32 capacity() const { return _arr.capacity(); }

	T* at(u32 i) { return (i >= 0 && i < _arr.size()) ? &_arr[i] : 0; }
	T* operator[](u32 i) { return _arr[i]; }

	T* front() { return _arr.front(); }
	T* back() { return _arr.back(); }

	T** begin() { return _arr.begin(); }
	T** end() { return _arr.end(); }

	array<T*> _arr;

	NOCOPY(list<T>);
};

#endif // UT_H