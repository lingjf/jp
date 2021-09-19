static inline h2_paragraph sentence_break(const h2_sentence& st, unsigned width)
{
   h2_paragraph paragraph;
   h2_string current_style;
   h2_sentence wrap;
   unsigned length = 0;

   for (auto& word : st) {
      if (h2_color::isctrl(word.c_str())) {  // + - style , issue
         wrap.push_back(word);
         current_style = word;
      } else {
         for (auto& c : word) {
            if (width <= length) {  // terminate line as later as possible
               paragraph.push_back(wrap);
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
   paragraph.push_back(wrap);
   return paragraph;
}

static inline h2_paragraph sentences_merge(const h2_paragraph& left_paragraph, const h2_paragraph& right_paragraph, unsigned left_width, unsigned right_width)
{
   h2_paragraph paragraph;
   const h2_string left_empty(left_width, ' '), right_empty(right_width, ' ');
   for (size_t i = 0; i < std::max(left_paragraph.size(), right_paragraph.size()); ++i) {
      auto left_wrap_sts = sentence_break(i < left_paragraph.size() ? left_paragraph[i] : left_empty, left_width);
      auto right_wrap_sts = sentence_break(i < right_paragraph.size() ? right_paragraph[i] : right_empty, right_width);
      for (size_t j = 0; j < std::max(left_wrap_sts.size(), right_wrap_sts.size()); ++j) {
         h2_sentence st;
         st += j < left_wrap_sts.size() ? left_wrap_sts[j].brush("reset") : color(left_empty, "reset");
         st.printf("dark gray", j < left_wrap_sts.size() - 1 ? "\\│ " : " │ ");
         st += j < right_wrap_sts.size() ? right_wrap_sts[j].brush("reset") : color(right_empty, "reset");
         st.printf("dark gray", j < right_wrap_sts.size() - 1 ? "\\" : " ");
         paragraph.push_back(st);
      }
   }
   return paragraph;
}

h2_inline h2_paragraph h2_layout::split(const h2_paragraph& left_paragraph, const h2_paragraph& right_paragraph, const char* left_title, const char* right_title, bool same, unsigned width)
{
   unsigned valid_width = width - (1 /* "|" */) - 1 /*|*/ - 4 /* spaces */;

   unsigned left_width = std::max(left_paragraph.width(), (unsigned)strlen(left_title)); /* at least title width */
   unsigned right_width = std::max(right_paragraph.width(), (unsigned)strlen(right_title));

   if (left_width < valid_width / 2)
      right_width = std::min(valid_width - left_width, right_width);
   else if (right_width < valid_width / 2)
      left_width = std::min(valid_width - right_width, left_width);
   else
      left_width = right_width = valid_width / 2;

   h2_sentence title = gray(h2_string(left_title).center(left_width)) + (same ? color(" = ", "green") : "   ") + gray(h2_string(right_title).center(right_width));
   h2_paragraph page = {title};

   if (left_paragraph.width() || right_paragraph.width())
      page += sentences_merge(left_paragraph, right_paragraph, left_width, right_width);

   return page;
}
