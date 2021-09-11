#ifndef __H2_UNIT_HPP__
#define __H2_UNIT_HPP__

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <cmath>
#include <string>
#include <vector>

#if defined _MSC_VER
#   include <malloc.h>
#   define alloca _alloca
#else
#   include <alloca.h>
#endif

#define h2_inline inline

struct h2_option
{
  bool caseless = false;
  bool fold_json = true;
} O;

#include "utils/h2_list.hpp"
#include "utils/h2_string.hpp"
#include "utils/h2_compare.hpp"
#include "utils/h2_sentence.hpp"
#include "utils/h2_paragraph.hpp"
#include "utils/h2_color.hpp"

#include "render/h2_layout.hpp"

#include "json/h2_json.hpp"

#endif
