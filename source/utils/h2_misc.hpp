
struct h2_pattern {
   static bool regex_match(const char* pattern, const char* subject, bool caseless = false);
   static bool wildcard_match(const char* pattern, const char* subject, bool caseless = false);
   static bool match(const char* pattern, const char* subject, bool caseless = false);
};

/* clang-format off */
#define h2_singleton(Class_) static Class_& I() { static Class_ i; return i; }
