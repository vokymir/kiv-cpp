module epic_files;

#include <filesystem>
#include <regex>
#include <string>
#include <vector>

std::vector<std::filesystem::path>
Find_Files_With_Extension(const std::filesystem::path &dir,
                          const std::string &extension) {
  if (!std::filesystem::is_directory(dir))
    return {};

  std::vector<std::filesystem::path> result;

  for (const auto &entry : std::filesystem::directory_iterator(dir)) {
    auto p = entry.path();
    if (p.has_extension() && p.extension() == extension)
      result.push_back(p);
  }

  return result;
}

std::vector<std::filesystem::path>
Find_Files_Containing(const std::filesystem::path &dir,
                      const std::string &pattern) {
  if (!std::filesystem::is_directory(dir))
    return {};

  const std::regex r(pattern);
  std::vector<std::filesystem::path> result;

  for (const auto &entry : std::filesystem::directory_iterator(dir)) {
    auto p = entry.path();
    std::string filename = p.filename().string();
    if (std::regex_match(filename, r))
      result.push_back(p);
  }

  return result;
}
