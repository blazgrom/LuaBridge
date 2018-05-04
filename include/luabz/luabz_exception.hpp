#pragma once
#include <stdexcept>
#include <string>
namespace luabz
{
/**
 * \brief A class used to report any kind of error that lua can generate
 *
 * Class used to report all error that can be generated from lua.\n
 * It's only used when the compiler flag USE_CPP_EXCEPTIONS is set to ON,
 * the default value is off.
 */
class luabz_exception : public std::runtime_error
{
  public:
    explicit luabz_exception(const std::string& msg) : std::runtime_error(msg) {}

    explicit luabz_exception(const char* msg) : std::runtime_error(msg) {}

    const char* what() const noexcept override { return std::runtime_error::what(); }
};
}  // namespace luabz