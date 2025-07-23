#include "exception/Exception.hpp"

using namespace tudov;

Exception::Exception(Context &context) noexcept
    : _context(context),
      _what(Default),
      std::exception(Default)
{
}

Exception::Exception(Context &context, std::string_view what) noexcept
    : _context(context),
      _what(what),
      std::exception(Default)
{
}

Context &Exception::GetContext() noexcept
{
	return _context;
}

const char *Exception::what() const noexcept
{
	return _what.data();
}

std::string &Exception::What() noexcept
{
	return _what;
}
