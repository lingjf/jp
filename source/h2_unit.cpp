#include "h2_unit.hpp"

#include <cassert>
#include <cctype>
#include <cstdarg>

#if defined _WIN32
#   ifndef NOMINMAX
#      define NOMINMAX  // fix std::min/max conflict with windows::min/max
#   endif
#   include <windows.h>
#define strcasecmp _stricmp
#else
#   include <sys/ioctl.h>
#   include <unistd.h>
#endif

#include "utils/h2_list.cpp"
#include "utils/h2_string.cpp"
#include "utils/h2_compare.cpp"
#include "utils/h2_line.cpp"
#include "utils/h2_color.cpp"

#include "json/h2_node.cpp"
#include "json/h2_lexical.cpp"
#include "json/h2_syntax.cpp"
#include "json/h2_select.cpp"
#include "json/h2_tree.cpp"
#include "json/h2_match.cpp"
#include "json/h2_dual.cpp"
#include "json/h2_json.cpp"

#include "render/h2_layout.cpp"
