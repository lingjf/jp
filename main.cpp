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
#   include <fnmatch.h>
#endif

#ifndef BINFO_GIT_COMMIT
#   define BINFO_GIT_COMMIT ""
#endif
#ifndef BINFO_DATE_TIME
#   define BINFO_DATE_TIME __TIME__ " " __DATE__
#endif

void usage()
{
   printf("\n");
   printf("\033[90mhttps://github.com/lingjf/\033[0m\033[32mjp\033[0m \033[90mv\033[0m1.2 \033[90m%s%s\033[0m \n\n", BINFO_GIT_COMMIT, BINFO_DATE_TIME);
   printf("\033[90mUsage:\033[0m \033[32mjp\033[0m a.json b.json");
   printf(" -\033[36mc\033[0m\033[90mase insensitive\033[0m");
   printf(" -\033[36mf\033[0m\033[90mold json\033[0m");
   printf(" -\033[36ms\033[0m\033[90melect '.c[1].name'\033[0m \n\n");
   printf("\033[90mUsage:\033[0m \033[32mjp\033[0m dir \033[90m scan json files in directory(s) then compare them in pairs \033[0m \n\n");
   printf("\033[90mUsage:\033[0m \033[32mjp\033[0m *.json \033[90m scan json files match wildcard then compare them in pairs \033[0m \n\n");

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

bool wildcard(const char* pattern, const char* subject, bool caseless)
{
#if defined _WIN32 || defined __CYGWIN__
   const char *scur = subject, *pcur = pattern;
   const char *sstar = nullptr, *pstar = nullptr;
   while (*scur) {
      if (*pcur == '?') {
         ++scur;
         ++pcur;
      } else if (*pcur == '*') {
         pstar = pcur++;
         sstar = scur;
      } else if (caseless ? ::tolower(*scur) == ::tolower(*pcur) : *scur == *pcur) {
         ++scur;
         ++pcur;
      } else if (pstar) {
         pcur = pstar + 1;
         scur = ++sstar;
      } else
         return false;
   }
   while (*pcur == '*') ++pcur;
   return !*pcur;
#else
   return !fnmatch(pattern, subject, caseless ? FNM_CASEFOLD : 0);
#endif
}

h2_string join(h2_string p1, h2_string p2)
{
   if (p1[p1.size() - 1] == SEP) {
      return p1 + p2;
   } else {
      return p1 + SEP + p2;
   }
}

h2_string fill(h2_string str)
{
   if (!(str.startswith("/") || str.startswith("\\") || str.startswith(".") || str.startswith("..")))
      str = "./" + str;
   return str;
}

h2_string first(h2_string str)
{
   for (char* p = (char*)str.c_str(); *p; p++) {
      if (*p == '/' || *p == '\\') *p = '\0';
   }
   return str.c_str();
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

bool is_file(const char* name)
{
#ifdef _MSC_VER
   DWORD dwAttr = GetFileAttributes(name);
   if (INVALID_FILE_ATTRIBUTES == dwAttr) return false;
   return dwAttr & FILE_ATTRIBUTE_NORMAL;
#else
   struct stat s;
   lstat(name, &s);
   return S_ISREG(s.st_mode);
#endif
}

static int scan_count = 0;
void scan_dir(h2_string& path, std::vector<h2_string>& files, const char* pattern = nullptr)
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
         scan_dir(p, files, pattern);
      } else {
         bool m = false;
         if (pattern) {
            m = wildcard(pattern, p.c_str(), true);
         } else {
            m = fn.endswith(".json", true);
         }
         if (m) {
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
         scan_dir(p, files, pattern);
      } else {
         bool m = false;
         if (pattern) {
            m = wildcard(pattern, p.c_str(), true);
         } else {
            m = fn.endswith(".json", true);
         }
         if (m) {
            files.push_back(p);
            scan_count++;
         }
      }
   }

   closedir(dir);
#endif
}

std::vector<h2_string> collect(std::vector<h2_string>& targets)
{
   std::vector<h2_string> files;
   for (auto it : targets) {
      if (is_dir(it.c_str())) {
         scan_dir(it, files, nullptr);
      } else if (strchr(it.c_str(), '*') || strchr(it.c_str(), '?')) {
         h2_string sf = fill(it);
         h2_string _1st = first(sf);
         scan_dir(_1st, files, sf.c_str());
      } else {  // file
         files.push_back(it);
      }
   }
   return files;
}

bool pair(h2_string& f0, h2_string& f1, h2_string selector, int seq, int cnt)
{
   h2_string j0 = read(f0), j1 = read(f1);

   if (selector.size()) j0 = h2_json::select(j0, selector, O.caseless);
   if (selector.size()) j1 = h2_json::select(j1, selector, O.caseless);
   bool same = h2_json::match(j0, j1, O.caseless) == 0;

   if (same && O.fold_json) j0 = "", j1 = "";

   h2_lines l0, l1;
   h2_json::diff(j0, j1, l0, l1, O.caseless);
   h2_lines page = h2_layout::split(l0, l1, f0.c_str(), f1.c_str(), same, width() - 1);
   for (auto& line : page) line.indent(1);

   if (seq) { // draw separate line
      h2_string t(page.width(), '-');
      printf("\033[90m%s\033[0m\n", t.c_str());
   }
   // if (same) {
   //    if (O.fold_json && (scan_count == 0) && cnt == 1) {
   //       printf("\033[32msame\033[0m.\n");
   //       return true;
   //    }
   // }
   h2_color::printl(page);
   printf("\n");
   return same;
}

int main(int argc, char** argv)
{
   const char* selector = "";
   std::vector<h2_string> targets;

#ifdef JPDOT
   targets.push_back(".");
#endif

   for (int i = 1; i < argc; i++) {
      if ('-' == argv[i][0]) {
         for (const char* j = &argv[i][1]; *j; j++) {
            if ('h' == *j || '?' == *j) {
               usage();
            } else if ('c' == *j) {
               O.caseless = true;
            } else if ('f' == *j) {
               O.fold_json = 0;
            } else if ('s' == *j) {
               if (i < argc - 1) selector = argv[++i];
            } else if ('-' == *j) {
            } else {
               printf("ignored invalid option: -%c \n", *j);
            }
         }
      } else {
         targets.push_back(argv[i]);
      }
   }

   if (targets.size() == 0) {
      usage();
   }

   std::vector<h2_string> files = collect(targets);

   if (files.size() < 2) files.push_back("");
   if (files.size() < 2) files.push_back("");

#if defined _MSC_VER
   SetConsoleOutputCP(65001);  // set console code page to utf-8
#endif

   int ret = 0, seq = 0, cnt = 0;
   for (int i = 0; i < files.size(); i++)
      for (int j = i + 1; j < files.size(); j++)
         cnt++;
   for (int i = 0; i < files.size(); i++)
      for (int j = i + 1; j < files.size(); j++)
         if (!pair(files[i], files[j], selector, seq++, cnt))
            ret = -1;

   return ret;
}
