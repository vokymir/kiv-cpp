export module epic_files;

#include <filesystem>
#include <string>
#include <vector>

export std::vector<std::filesystem::path>
Find_Files_With_Extension(const std::filesystem::path &dir,
                          const std::string &extension);

export std::vector<std::filesystem::path>
Find_Files_Containing(const std::filesystem::path &dir,
                      const std::string &pattern);
