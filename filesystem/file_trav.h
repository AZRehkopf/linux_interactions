#include <cstdio>
#include <string>
#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <libgen.h>

DIR *opendir(const char *name);

using namespace std;