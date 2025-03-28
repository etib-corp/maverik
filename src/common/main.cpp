#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

int main(void)
{
    std::vector<const char *> extensions;
    const std::vector<std::string> kPlatformExtensions = {
        ".so",
        ".dll",
        ".dylib",
    };
  std::transform(kPlatformExtensions.begin(),
                 kPlatformExtensions.end(),
                 std::back_inserter(extensions),
                 [](const std::string &ext) { return ext.c_str(); });

    for (const auto &ext : extensions) {
        std::cout << ext << std::endl;
    }
    return 0;
}
