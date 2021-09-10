
h2_inline bool h2_pattern::regex_match(const char* pattern, const char* subject, bool caseless)
{
   bool result = false;
   try {  // c++11 support regex; gcc 4.8 start support regex, gcc 5.5 icase works.
      result = std::regex_match(subject, caseless ? std::regex(pattern, std::regex::icase) : std::regex(pattern));
   } catch (const std::regex_error&) {
      result = false;
   }
   return result;
}

h2_inline bool h2_pattern::wildcard_match(const char* pattern, const char* subject, bool caseless)
{
   const char *scur = subject, *pcur = pattern;
   const char *sstar = nullptr, *pstar = nullptr;
   while (*scur) {
      if (*pcur == '?') {
         ++scur;
         ++pcur;
      } else if (*pcur == '*') {
         pstar = pcur++;
         sstar = scur;
      } else if (caseless ? ::tolower(*scur) == ::tolower(*pcur) : *scur == *pcur) {
         ++scur;
         ++pcur;
      } else if (pstar) {
         pcur = pstar + 1;
         scur = ++sstar;
      } else
         return false;
   }
   while (*pcur == '*') ++pcur;
   return !*pcur;
}

h2_inline bool h2_pattern::match(const char* pattern, const char* subject, bool caseless)
{
   return wildcard_match(pattern, subject, caseless) || regex_match(pattern, subject, caseless);
}

static inline const char* comma_if(bool a, const char* t = ", ", const char* f = "") { return a ? t : f; };

#define h2_sprintvf(str, fmt, ap)               \
   do {                                         \
      va_list bp;                               \
      va_copy(bp, ap);                          \
      int len = vsnprintf(nullptr, 0, fmt, bp); \
      str = (char*)alloca(len + 1);             \
      va_end(bp);                               \
      len = vsnprintf(str, len + 1, fmt, ap);   \
   } while (0)

#define h2_sprintf(str, fmt)     \
   do {                          \
      va_list ap;                \
      va_start(ap, fmt);         \
      h2_sprintvf(str, fmt, ap); \
      va_end(ap);                \
   } while (0)
