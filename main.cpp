#include "h2unit.h"
#include <iostream>
#include <fstream>
#include <sstream>

#ifdef _MSC_VER
#include <windows.h>
#define SEP '\\'
#else
#define SEP '/'
#include <sys/stat.h>
#include <dirent.h>
#endif

void usage()
{
   printf("\n");
   printf("\033[34;4mhttps://github.com/lingjf/jp\033[0m v1.3 \n\n");
   printf("\033[90mUsage: \033[0m \033[32mjp\033[0m 1.json 2.json");
   printf(" -\033[36mc\033[0m\033[90mase insensitive\033[0m");
   printf(" -\033[36mf\033[0m\033[90mold json\033[0m");
   printf(" -\033[36mp\033[0m\033[90mrogramming json\033[0m");
   printf(" -\033[36ms\033[0m\033[90melect '.a[1]'\033[0m \n\n");
   printf("\033[90mUsage: \033[0m \033[32mjp\033[0m dir \033[90m scan json files in directory then compare them in pairs \033[0m \n\n");

   exit(0);
}

unsigned cww()
{
   unsigned t = 120;
#if defined _WIN32
   //TODO get PowerShell width
#else
   struct winsize w;
   if (-1 != ioctl(STDOUT_FILENO, TIOCGWINSZ, &w)) t = 16 < w.ws_col ? w.ws_col : 120;
#endif
   return t;
}

h2_string read(h2_string& fn)
{
   if (fn.size() == 0) return "";
   std::ifstream is(fn);
   if (!is.is_open()) {
      printf("jp \033[33merror:\033[0m Couldn't open %s \n\n", fn.c_str());
      exit(-1);
   }
   std::ostringstream sstr;
   sstr << is.rdbuf();
   return sstr.str();
}

h2_string join(h2_string p1, h2_string p2)
{
   if (p1[p1.size() - 1] == SEP) {
      return p1 + p2;
   } else {
      return p1 + SEP + p2;
   }
}

bool is_dir(const char* name)
{
#ifdef _MSC_VER
   DWORD dwAttr = GetFileAttributes(name);
   if (INVALID_FILE_ATTRIBUTES == dwAttr) return false;
   return dwAttr & FILE_ATTRIBUTE_DIRECTORY;
#else 
   struct stat s;
   lstat(name, &s);
   return S_ISDIR(s.st_mode);
#endif
}

void scan_dir(h2_string& path, std::vector<h2_string>& files)
{
#ifdef _MSC_VER
   HANDLE h;
   WIN32_FIND_DATA fd;
   LARGE_INTEGER size;
   h2_string dir = join(path, "*");
   h = FindFirstFile(dir.c_str(), &fd);
   if (h == INVALID_HANDLE_VALUE) return;
   do {
      if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
         continue;
      h2_string fn = fd.cFileName;
      h2_string p = join(path, fn);
      if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
         scan_dir(p, files);
      } else {
         if (fn.endswith(".json", true)) files.push_back(p);
      }
   } while (FindNextFile(h, &fd));
#else
   struct dirent* dp;
   DIR* dir = opendir(path.c_str());
   if (!dir) return;

   while ((dp = readdir(dir))) {
      if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
         continue;
      h2_string fn = dp->d_name;
      h2_string p = join(path, fn);
      if (dp->d_type == DT_DIR) {
         scan_dir(p, files);
      } else {
         if (fn.endswith(".json", true)) files.push_back(p);
      }
   }

   closedir(dir);
#endif
}

bool compare(h2_string& e_value, h2_string& a_value, h2_string selector)
{
   if (selector.size()) e_value = h2_json::select(e_value, selector, O.caseless);
   if (selector.size()) a_value = h2_json::select(a_value, selector, O.caseless);
   return 0==h2_json::match(e_value, a_value, O.caseless);
}

void print(h2_string e_value, h2_string a_value, h2_string ef, h2_string af)
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
      h2_rows rows = h2_layout::split(e_rows, a_rows, ef.c_str(), af.c_str(), 0, 'd', cww() - 1);
      for (auto& row : rows) row.indent(1);
      h2_color::printl(rows);
   }
}

bool pair(h2_string& f0, h2_string& f1, h2_string selector)
{
   h2_string j0 = read(f0), j1 = read(f1);
   bool ret = compare(j0, j1, selector);
   print(j0, j1, f0, f1);
   return ret;
}

int main(int argc, char** argv)
{
   const char* selector = "";
   std::vector<h2_string> paths;

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
         paths.push_back(argv[i]);
      }
   }

   if (paths.size() == 0) {
      usage();
   }

   std::vector<h2_string> files;

   for (auto it : paths) {
      if (is_dir(it.c_str())) {
         scan_dir(it, files);
      } else {
         files.push_back(it);
      }
   }

   if (files.size() < 2) files.push_back("");
   if (files.size() < 2) files.push_back("");

#if defined _MSC_VER
   SetConsoleOutputCP(65001);  // set console code page to utf-8
#endif

   int ret = 0;
   for (int i = 0; i < files.size(); i++)
      for (int j = i + 1; j < files.size(); j++)
         if (!pair(files[i], files[j], selector))
            ret = 1;
   
   return ret;
}
