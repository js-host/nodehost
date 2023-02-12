#pragma once

#ifndef NODEHOST_INTERNAL_UTILS_H
#define NODEHOST_INTERNAL_UTILS_H

#include <memory>
#include <vector>
#include <algorithm>
#include <string>

#define PIMPL_CHECK_DISPOSED(name) if (this->disposed()) throw node_error("Attempt to access disposed " name)

namespace nodehost {
	namespace internal {

		// source: https://dev.krzaq.cc/post/you-dont-need-a-stateful-deleter-in-your-unique_ptr-usually/
		template<auto func>
		struct functor
		{
			template<typename... Us>
			auto operator()(Us&&... us) const {
				return func(std::forward<Us...>(us...));
			}
		};

		// remove all expired weak_ptr
		template <typename T>
		void gc_weak_ptr_vector(std::vector<std::weak_ptr<T>>& v) {
			v.erase(
				std::remove_if(v.begin(), v.end(), [](std::weak_ptr<T> ptr) { return ptr.expired(); }),
				v.end()
			);
		}

		template<typename... Args>
		std::string string_format(const std::string& format, Args... args )
		{
			int size_s = std::snprintf( nullptr, 0, format.c_str(), args...) + 1;
			if (size_s <= 0) {
				throw std::runtime_error("snprintf failed");
			}
			auto size = static_cast<size_t>(size_s);
			std::unique_ptr<char[]> buf(new char[size]);
			std::snprintf(buf.get(), size, format.c_str(), args...);
			return std::string(buf.get(), buf.get() + size - 1);
		}

	}
}

#endif
