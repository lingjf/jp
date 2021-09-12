#include "./source/h2_unit.cpp"

#include <fstream>
#include <sstream>

#ifdef _MSC_VER
#   include <windows.h>
#   define SEP '\\'
#else
#   define SEP '/'
#   include <sys/stat.h>
#   include <dirent.h>
#endif

void usage()
{
   printf("\n");
   printf("\033[90;4mhttps://github.com/lingjf/\033[0m\033[32mjp\033[0m \033[90mv\033[0m1.2 \n\n");
   printf("\033[90mUsage:\033[0m \033[32mjp\033[0m a.json b.json");
   printf(" -\033[36mc\033[0m\033[90mase insensitive\033[0m");
   printf(" -\033[36mf\033[0m\033[90mold json\033[0m");
   printf(" -\033[36ms\033[0m\033[90melect '.c[1].name'\033[0m \n\n");
   printf("\033[90mUsage:\033[0m \033[32mjp\033[0m dir \033[90m scan json files in directory(s) then compare them in pairs \033[0m \n\n");

   exit(0);
}

unsigned width()
{
   unsigned t = 120;
#if defined _WIN32
   CONSOLE_SCREEN_BUFFER_INFO csbi;
   int columns, rows;

   GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
   columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
   rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

   t = 16 < columns ? columns : 120;
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

static int scan_count = 0;
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
         if (fn.endswith(".json", true)) {
            files.push_back(p);
            scan_count++;
         }
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
         if (fn.endswith(".json", true)) {
            files.push_back(p);
            scan_count++;
         }
      }
   }

   closedir(dir);
#endif
}

bool pair(h2_string& f0, h2_string& f1, h2_string selector, int seq)
{
   h2_string j0 = read(f0), j1 = read(f1);

   if (selector.size()) j0 = h2_json::select(j0, selector, O.caseless);
   if (selector.size()) j1 = h2_json::select(j1, selector, O.caseless);
   bool same = h2_json::match(j0, j1, O.caseless) == 0;

   if (same && O.fold_json) j0 = "", j1 = "";

   h2_paragraph r0, r1;
   h2_json::diff(j0, j1, r0, r1, O.caseless);
   h2_paragraph page = h2_layout::split(r0, r1, f0.c_str(), f1.c_str(), same, width() - 1);
   for (auto& st : page) st.indent(1);

   if (seq) {
      h2_string t(page.width(), '-');
      printf("\033[90m%s\033[0m\n", t.c_str());
   }
   if (same) {
      if (O.fold_json && scan_count == 0) {
         printf("\033[32msame\033[0m.\n");
         return true;
      }
   }
   h2_color::printl(page);
   return same;
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
               O.caseless = true;
            } else if ('f' == *j) {
               O.fold_json = !O.fold_json;
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

   for (auto it : paths)
      if (is_dir(it.c_str()))
         scan_dir(it, files);
      else
         files.push_back(it);

   if (files.size() < 2) files.push_back("");
   if (files.size() < 2) files.push_back("");

#if defined _MSC_VER
   SetConsoleOutputCP(65001);  // set console code page to utf-8
#endif

   int ret = 0, seq = 0;
   for (int i = 0; i < files.size(); i++)
      for (int j = i + 1; j < files.size(); j++)
         if (!pair(files[i], files[j], selector, seq++))
            ret = 1;

   return ret;
}
