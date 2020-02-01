#pragma once

#include <memory>
#include <stdexcept>

#define ML_ASSERT(cond, msg) if( !(cond) ) throw std::runtime_error("Assertion failed: " msg);
namespace Monolight
{
	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T>
	using Hold = std::unique_ptr<T>;
}
