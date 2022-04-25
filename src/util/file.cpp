#include "util/file.hpp"
#include <limits.h>
#include <spdlog/spdlog.h>

namespace mg8
{
  std::string get_filepath(const char *path)
  {
    char buf[PATH_MAX] = {};

    char *res = realpath(path, buf);
    if (!res)
    {
      spdlog::error("Filepath >{}< error resolving with realpath, error: {}", path, strerror(errno));
      return std::string();
    }

    return std::string(buf);
  }
}