#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

int lsd(const char *path)
{
  DIR *dp;
  struct dirent *ep;
  struct stat st_buf;
  //  char *subpath;
  //  int pathLen = strlen(path);
  char fullpath[255];
  char cwd[255];
  getcwd(cwd, 255);

  dp = opendir(path);
  if(dp != NULL)
  {
    if(chdir(path) < 0)
    {
      cerr << "chdir: " << path << endl;
      exit(1);
    }
    while(((ep = readdir (dp)) != NULL)
	  && (strcmp(ep->d_name, ".") != 0)
	  && (strcmp(ep->d_name, "..") != 0))
    {      
      // Get the status of the file system object.
      if (stat(ep->d_name, &st_buf) != 0)
      {
	cerr << "Error: " << strerror(errno) << ", " << ep->d_name
	     << "cwd: " << get_current_dir_name() << endl;
	exit(1);
      }
      
      if(S_ISDIR(st_buf.st_mode))
      {
	//	subpath = new char[pathLen + strlen(ep->d_name) + 1];
	//	subpath = strcat(strcat(strcpy(subpath, path), "/"), ep->d_name);
	
	cout << getcwd(fullpath, 255) << "/" << ep->d_name << endl;
	lsd(ep->d_name);
      }
    }
    (void) closedir(dp);
  }
  else
    cerr << "Couldn't open the directory: " << path << endl;
  if(chdir(cwd) < 0)
  {
    cerr << "chdir: " << cwd << endl;
    exit(1);
  }
  return 0;
}

int main(int argc, char *argv[])
{
  // Ensure argument passed.
  if (argc < 2)
  {
    cout << "Usage: progName <fileSpec>\n";
    cout << "       where <fileSpec> is the dir to copy.\n";
    return 1;
  }

  for(int i = 1; i < argc; i++)
  {
    cout << i << ":" << argv[i] << endl;
    lsd(argv[i]);
  }
  return 0;
}
