#include "h2unit.h"
#include <iostream>
#include <fstream>
#include <sstream>

bool compare(h2_string& e_value, h2_string& a_value, h2_string selector)
{
   if (selector.size()) e_value = h2_json::select(e_value, selector, O.caseless);
   if (selector.size()) a_value = h2_json::select(a_value, selector, O.caseless);
   return 0 == h2_json::match(e_value, a_value, O.caseless);
}

void print(h2_string e_value, h2_string a_value, unsigned cww)
{
   h2_rows e_rows, a_rows;
   if (O.copy_paste_json || !h2_json::diff(e_value, a_value, e_rows, a_rows, O.caseless)) {
      e_rows = h2_json::format(e_value);
      a_rows = h2_json::format(a_value);
      for (size_t i = 0; i < e_rows.size(); ++i)
         if (i) e_rows[i].indent(8);
      for (size_t i = 0; i < a_rows.size(); ++i)
         if (i) a_rows[i].indent(8);
      h2_color::prints("dark gray", "expect");
      h2_color::prints("green", "> ");
      h2_color::printl(e_rows);
      h2_color::prints("dark gray", "actual");
      h2_color::prints("red", "> ");
      h2_color::printl(a_rows);
   } else {
      h2_rows rows = h2_layout::split(e_rows, a_rows, h2_option::I().path[0], h2_option::I().path[1], 0, 'd', cww - 1);
      for (auto& row : rows) row.indent(1);
      h2_color::printl(rows);
   }
}

std::string slurp(std::ifstream& in) 
{
   std::ostringstream sstr;
   sstr << in.rdbuf();
   return sstr.str();
}

void usage()
{
   printf("\033[32mjp\033[0m v1.1 \033[34;4mhttps://github.com/lingjf/jp\033[0m \n\n");
   printf("Usage: \033[32mjp\033[0m 1.json 2.json");
   printf(" -\033[36mc\033[0m\033[90mase insensitive\033[0m");
   printf(" -\033[36mf\033[0m\033[90mold json\033[0m");
   printf(" -\033[36mp\033[0m\033[90mrogramming json\033[0m");
   printf(" -\033[36ms\033[0m\033[90melect '.a[1]'\033[0m \n\n");
   exit(0);
}

int main(int argc, char** argv)
{
   const char* selector = "";
   int n = 0;
   for (int i = 1; i < argc; i++) {
      if ('-' == argv[i][0]) {
         for (const char* j = &argv[i][1]; *j; j++) {
            if ('h' == *j || '?' == *j) {
               usage();
            } else if ('c' == *j) {
               h2_option::I().caseless = true;
            } else if ('f' == *j) {
               h2_option::I().fold_json = !h2_option::I().fold_json;
            } else if ('p' == *j || 'y' == *j) {
               h2_option::I().copy_paste_json = true;
            } else if ('s' == *j) {
               if (i < argc - 1) selector = argv[++i];
            } else if ('-' == *j) {
               
            } else {
               printf("ignored invalid option: -%c \n", *j);
            }
         }
      } else {
         strcpy(h2_option::I().path[n++], argv[i]);
      }
   }

   if (n != 2) usage();

   unsigned cww = 120;
#if defined _WIN32
   //TODO get PowerShell width
#else
   struct winsize w;
   if (-1 != ioctl(STDOUT_FILENO, TIOCGWINSZ, &w)) cww = 16 < w.ws_col && w.ws_col <= 120 ? w.ws_col : 120;
#endif

#if defined _MSC_VER
   SetConsoleOutputCP(65001);  // set console code page to utf-8
#endif

   std::ifstream s0(h2_option::I().path[0]);
   std::ifstream s1(h2_option::I().path[1]);
   if (!s0.is_open() || !s1.is_open()) {
      if (!s0.is_open()) std::cerr << "Couldn't open " << h2_option::I().path[0] << std::endl;
      if (!s1.is_open()) std::cerr << "Couldn't open " << h2_option::I().path[1] << std::endl;
      return -1;
   }
   auto j0 = slurp(s0);
   auto j1 = slurp(s1);

   h2_string e = j0, a = j1;
   if (compare(e, a, selector)) {
      printf("same.\n");
      return 0;
   }
   print(e, a, cww);

   return 1;
}
