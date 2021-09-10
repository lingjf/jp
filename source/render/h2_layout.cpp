static inline h2_rows row_break(const h2_row& row, unsigned width)
{
   h2_rows rows;
   h2_string current_style;
   h2_row wrap;
   unsigned length = 0;

   for (auto& word : row) {
      if (h2_color::isctrl(word.c_str())) {  // + - style , issue
         wrap.push_back(word);
         current_style = word;
      } else {
         for (auto& c : word) {
            if (width <= length) {  // terminate line as later as possible
               rows.push_back(wrap);
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
   rows.push_back(wrap);
   return rows;
}

static inline void rows_merge(h2_rows& rows, const h2_rows& left_rows, const h2_rows& right_rows, unsigned left_width, unsigned right_width)
{
   const h2_string left_empty(left_width, ' '), right_empty(right_width, ' ');
   for (size_t i = 0; i < std::max(left_rows.size(), right_rows.size()); ++i) {
      auto left_wrap_rows = row_break(i < left_rows.size() ? left_rows[i] : left_empty, left_width);
      auto right_wrap_rows = row_break(i < right_rows.size() ? right_rows[i] : right_empty, right_width);
      for (size_t j = 0; j < std::max(left_wrap_rows.size(), right_wrap_rows.size()); ++j) {
         h2_row row;
         row += j < left_wrap_rows.size() ? left_wrap_rows[j].brush("reset") : color(left_empty, "reset");
         row.printf("dark gray", j < left_wrap_rows.size() - 1 ? "\\│ " : " │ ");
         row += j < right_wrap_rows.size() ? right_wrap_rows[j].brush("reset") : color(right_empty, "reset");
         row.printf("dark gray", j < right_wrap_rows.size() - 1 ? "\\" : " ");
         rows.push_back(row);
      }
   }
}

h2_inline h2_rows h2_layout::split(const h2_rows& left_rows, const h2_rows& right_rows, const char* left_title, const char* right_title, bool same, unsigned width)
{
   unsigned valid_width = width - (1 /* "|" */) - 1 /*|*/ - 4 /* spaces */;

   unsigned left_width = std::max(left_rows.width(), (unsigned)strlen(left_title)); /* at least title width */
   unsigned right_width = std::max(right_rows.width(), (unsigned)strlen(right_title));

   if (left_width < valid_width / 2)
      right_width = std::min(valid_width - left_width, right_width);
   else if (right_width < valid_width / 2)
      left_width = std::min(valid_width - right_width, left_width);
   else
      left_width = right_width = valid_width / 2;

   h2_rows rows;
   h2_row title_row = gray(h2_string(left_title).center(left_width)) + (same ? color(" = ", "green") : "   ") + gray(h2_string(right_title).center(right_width));
   rows.push_back(title_row);

   rows_merge(rows, left_rows, right_rows, left_width, right_width);
   return rows;
}
