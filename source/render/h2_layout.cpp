static inline h2_lines line_break(const h2_line& line, size_t width)
{
   h2_lines lines;
   h2_string current_style;
   h2_line wrap;
   size_t length = 0;

   for (auto& word : line) {
      if (h2_color::isctrl(word.c_str())) {  // + - style , issue
         wrap.push_back(word);
         current_style = word;
      } else {
         for (auto& c : word) {
            if (width <= length) {  // terminate line as later as possible
               lines.push_back(wrap);
               wrap.clear();
               length = 0;
               if (current_style.size()) wrap.push_back(current_style);
            }
            wrap.push_back(h2_string(1, c));
            ++length;
         }
      }
   }
   if (length < width) wrap.push_back(h2_string(width - length, ' '));
   lines.push_back(wrap);
   return lines;
}

static inline h2_lines lines_merge(const h2_lines& left_lines, const h2_lines& right_lines, size_t left_width, size_t right_width)
{
   h2_lines lines;
   const h2_string left_empty(left_width, ' '), right_empty(right_width, ' ');
   for (size_t i = 0; i < std::max(left_lines.size(), right_lines.size()); ++i) {
      auto left_wrap_lines = line_break(i < left_lines.size() ? left_lines[i] : left_empty, left_width);
      auto right_wrap_lines = line_break(i < right_lines.size() ? right_lines[i] : right_empty, right_width);
      for (size_t j = 0; j < std::max(left_wrap_lines.size(), right_wrap_lines.size()); ++j) {
         h2_line line;
         line += j < left_wrap_lines.size() ? left_wrap_lines[j].brush("reset") : color(left_empty, "reset");
         line.printf("dark gray", j < left_wrap_lines.size() - 1 ? "\\│ " : " │ ");
         line += j < right_wrap_lines.size() ? right_wrap_lines[j].brush("reset") : color(right_empty, "reset");
         line.printf("dark gray", j < right_wrap_lines.size() - 1 ? "\\" : " ");
         lines.push_back(line);
      }
   }
   return lines;
}

h2_inline h2_lines h2_layout::split(const h2_lines& left_lines, const h2_lines& right_lines, const char* left_title, const char* right_title, bool same, size_t width)
{
   size_t valid_width = width - (1 /* "|" */) - 1 /*|*/ - 4 /* spaces */;

   size_t left_width = std::max(left_lines.width(), strlen(left_title)); /* at least title width */
   size_t right_width = std::max(right_lines.width(), strlen(right_title));

   if (left_width < valid_width / 2)
      right_width = std::min(valid_width - left_width, right_width);
   else if (right_width < valid_width / 2)
      left_width = std::min(valid_width - right_width, left_width);
   else
      left_width = right_width = valid_width / 2;

   h2_line title = gray(h2_string(left_title).center(left_width)) + (same ? color(" = ", "green") : "   ") + gray(h2_string(right_title).center(right_width));
   h2_lines lines = {title};

   if (left_lines.width() || right_lines.width())
      lines += lines_merge(left_lines, right_lines, left_width, right_width);

   return lines;
}
