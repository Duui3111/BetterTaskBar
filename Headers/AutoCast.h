#pragma once

template<class T>
class AutoCast
{
public:
	explicit AutoCast(T any) noexcept : _any(any) {}
	template <typename T> T ToAuto() noexcept { return (T)_any; }
	template <typename T> T ToStatic() noexcept { return static_cast<T>(_any); }
	template <typename T> T ToConst() noexcept { return const_cast<T>(_any); }
	template <typename T> T ToReinterpret() noexcept { return reinterpret_cast<T>(_any); }
	template <typename T> operator T*() const noexcept { return (T*)_any; }
	template <typename T> operator T() const noexcept { return (T)_any; }

private:
	T _any;
};