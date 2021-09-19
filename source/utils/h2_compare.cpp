
struct h2_fuzzy {
   static unsigned int levenshtein(const char* s1, const char* s2, int n1, int n2, bool caseless)
   {
      std::vector<unsigned int> col(n2 + 1), prevCol(n2 + 1);
      for (unsigned int i = 0; i < prevCol.size(); i++)
         prevCol[i] = i;
      for (unsigned int i = 0; i < n1; i++) {
         col[0] = i + 1;
         for (unsigned int j = 0; j < n2; j++) {
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
      int n1 = strlen(s1), n2 = strlen(s2);
      if (!n1 && !n2) return 1;
      double ed = levenshtein(s1, s2, n1, n2, caseless);
      return 1.0 - ed / ((n1 + n2) * 0.5);
   }
};
