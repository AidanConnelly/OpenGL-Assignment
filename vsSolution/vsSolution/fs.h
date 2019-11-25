#ifdef __MINGW32__
#include <filesystem>
#define filesystem std::filesystem
#endif
#ifdef _MSC_VER
#include <experimental/filesystem>
#define filesystem std::experimental::filesystem
#endif