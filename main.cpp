#include "h2unit.h"

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

void usage()
{
   printf("\033[32mjp\033[0m v1.0 \033[34;4mhttps://github.com/lingjf/jp\033[0m \n\n");
   printf("Usage: \033[32mjp\033[0m 1.json 2.json");
   printf(" -\033[36mc\033[0m\033[90maseless\033[0m");
   printf(" -\033[36mf\033[0m\033[90mold json\033[0m");
   printf(" -\033[36mp\033[0m\033[90maste copy\033[0m");
   printf(" -\033[36ms\033[0m\033[90melector .a[1]\033[0m \n\n");
   exit(0);
}

static char a1[1024 * 1024 * 8];
static char a2[1024 * 1024 * 8];

int main(int argc, char** argv)
{
   const char* selector = "";

   for (int i = 1, j = 0; i < argc; i++) {
      if (strcmp("-h", argv[i]) == 0) {
         usage();
      } else if (strcmp("-c", argv[i]) == 0) {
         h2_option::I().caseless = true;
      } else if (strcmp("-f", argv[i]) == 0) {
         h2_option::I().fold_json = true;
      } else if (strcmp("-y", argv[i]) == 0) {
         h2_option::I().copy_paste_json = true;
      } else if (strcmp("-s", argv[i]) == 0) {
         if (i < argc - 1) selector = argv[++i];
      } else {
         strcpy(h2_option::I().path[j++], argv[i]);
      }
   }

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
   FILE* f1 = fopen(h2_option::I().path[0], "r");
   FILE* f2 = fopen(h2_option::I().path[1], "r");
   if (!f1 || !f2) {
      if (!f1) printf("Couldn't open %s\n", h2_option::I().path[0]);
      if (!f2) printf("Couldn't open %s\n", h2_option::I().path[1]);
      exit(-1);
   }
   size_t r1 = fread(a1, 1, sizeof(a1), f1);
   size_t r2 = fread(a2, 1, sizeof(a2), f2);
   a1[r1] = '\0';
   a2[r2] = '\0';
   fclose(f1);
   fclose(f2);

   h2_string e = a1, a = a2;
   if (compare(e, a, selector)) {
      printf("same.\n");
      return 0;
   } 
   print(e, a, cww);

   return 1;
}
