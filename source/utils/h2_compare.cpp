
struct h2_fuzzy {
   static size_t levenshtein(const char* s1, const char* s2, size_t n1, size_t n2, bool caseless)
   {
      std::vector<size_t> col(n2 + 1), prevCol(n2 + 1);
      for (std::vector<size_t>::size_type i = 0; i < prevCol.size(); i++)
         prevCol[i] = (int)i;
      for (size_t i = 0; i < n1; i++) {
         col[0] = i + 1;
         for (size_t j = 0; j < n2; j++) {
            bool eq = caseless ? ::tolower(s1[i]) == ::tolower(s2[j]) : s1[i] == s2[j];
            col[j + 1] = std::min(std::min(1 + col[j], 1 + prevCol[1 + j]), prevCol[j] + (eq ? 0 : 1));
         }
         col.swap(prevCol);
      }
      return prevCol[n2];
   }

   // 1 absolute match, 0 absolute not match
   static double similarity(const char* s1, const char* s2, bool caseless)
   {
      size_t n1 = strlen(s1), n2 = strlen(s2);
      if (!n1 && !n2) return 1;
      return 1.0 - levenshtein(s1, s2, n1, n2, caseless) / ((n1 + n2) * 0.5);
   }
};
