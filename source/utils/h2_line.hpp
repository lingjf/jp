struct h2_line : std::vector<h2_string> {
   h2_line() {}
   h2_line(const char* a) : std::vector<h2_string>({a}) {}
   h2_line(const h2_string& a) : std::vector<h2_string>({a}) {}
   h2_line(std::initializer_list<h2_string> il) : std::vector<h2_string>(il) {}

   size_t width(bool ignore_indent = false) const;
   h2_line& indent(size_t n, const char c = ' ');
   h2_line& padding(size_t n, const char c = ' ');

   h2_line& printf(const char* style, const char* format, ...);
   h2_line& operator+=(const h2_line& line);
   h2_line& brush(const char* style);

   bool enclosed(const char c) const;
   h2_line gray_quote() const;
   h2_line acronym(size_t width = 16, size_t tail = 0) const;
   h2_string string() const;

   static void samesizify(h2_line& a, h2_line& b);
};

inline h2_line operator+(const h2_line& a, const h2_line& b)  // implicit conversion const char* / h2_string
{
   h2_line line;
   line.insert(line.end(), a.begin(), a.end());
   line.insert(line.end(), b.begin(), b.end());
   return line;
}

static inline const h2_line color(const h2_line& line, const char* style) { return ("\033{" + h2_string(style) + "}") + line + "\033{reset}"; }
static inline const h2_line delta(const h2_line& line, const char* style) { return ("\033{+" + h2_string(style) + "}") + line + ("\033{-" + h2_string(style) + "}"); }
static inline const h2_line gray(const h2_line& line) { return delta(line, "dark gray"); }

///// h2_lines
struct h2_lines : std::vector<h2_line> {
   h2_lines() {}
   h2_lines(std::initializer_list<h2_line> il) : std::vector<h2_line>(il) {}

   h2_lines& operator+=(const h2_lines& lines);

   size_t width() const;
   bool foldable(size_t width = 20) const;
   h2_line folds() const;

   h2_string string() const;

   void sequence(size_t indent = 0, size_t start = 0);
   static void samesizify(h2_lines& a, h2_lines& b);
};
