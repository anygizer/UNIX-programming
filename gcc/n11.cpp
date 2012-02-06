#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace std;

bool isdir(const char *path)
{
  struct stat st_buf;
  // Get the status of the file system object.
  if (stat(path, &st_buf) != 0)
  {
    cerr << "Error: " << strerror(errno) << ", " << path
	 << "cwd: " << get_current_dir_name() << endl;
    exit(1);
  }
  return S_ISDIR(st_buf.st_mode);
}

int rm(const char *path)
{
  if(isdir(path))
  {
    char cwd[255];
    getcwd(cwd, 255);
    DIR *dp = opendir(path);
    struct dirent *ep;
    if(dp != NULL)
    {
      if(chdir(path) < 0)
      {
	cerr << "chdir(in): " << path << endl;
	exit(1);
      }
      while((ep = readdir(dp)) != NULL)
      {
	if((strcmp(ep->d_name, ".") != 0) && (strcmp(ep->d_name, "..") != 0))
	{
	  rm(ep->d_name);
	}
      }
      if(chdir(cwd) < 0)
      {
	cerr << "chdir(out): " << cwd << endl;
	exit(1);
      }
      (void) closedir(dp);
      return rmdir(path);
    }
    else
    {
      exit(1);
    }
  }
  else
  {
    return unlink(path);
  }
}

int cleardir(const char* dirpath)
{
  struct stat st_buf;
  // Get the status of the file system object.
  if (stat(dirpath, &st_buf) != 0)
  {
    cerr << "Error: " << strerror(errno) << ", " << dirpath
	 << "cwd: " << get_current_dir_name() << endl;
    exit(1);
  }
  if(S_ISDIR(st_buf.st_mode))
  {
    rm(dirpath);
    if(mkdir(dirpath, st_buf.st_mode) != 0)
    {
      cerr << "Error: " << strerror(errno) << ", " << dirpath
	   << "cwd: " << get_current_dir_name() << endl;
      exit(1);
    }
    return 0;
  }
  else
  {
    exit(1);
  }
}

int cpdirtree(const char *dst, const char *src)
{
  // Get the current directory to know where to return in the end.
  char cwd[255];
  getcwd(cwd, 255);

  DIR *dp;
  struct dirent *ep;
  char *subDst;
  int dstLen = strlen(dst);
  char absoluteSrc[255];

  struct stat st_buf;

  dp = opendir(src);
  if(dp != NULL)
  {
    if(chdir(src) < 0)
    {
      cerr << "chdir(in): " << src << endl;
      exit(1);
    }
    while((ep = readdir(dp)) != NULL)
    {
      if((strcmp(ep->d_name, ".") == 0) || (strcmp(ep->d_name, "..") == 0))
	continue;
      // Get the status of the file system object.
      if (stat(ep->d_name, &st_buf) != 0)
      {
	cerr << "Error: " << strerror(errno) << ", " << ep->d_name
	     << "cwd: " << get_current_dir_name() << endl;
	exit(1);
      }

      if(S_ISDIR(st_buf.st_mode))
      {
	getcwd(absoluteSrc, 255);

	cout << absoluteSrc << "/" << ep->d_name << endl;

	if(chdir(dst) < 0)
	{
	  cerr << "chdir(indst): " << dst << endl;
	  exit(1);
	}
	//Checking for existance
	if((mkdir(ep->d_name, st_buf.st_mode) != 0) && (errno != EEXIST))
	{
	  cerr << "Error: " << strerror(errno) << ", " << ep->d_name
	       << "cwd: " << get_current_dir_name() << endl;
	  exit(1);
	}
	if(chdir(absoluteSrc) < 0)
	{
	  cerr << "chdir(outdst): " << absoluteSrc << endl;
	  exit(1);
	}

	if(dst[dstLen-1] == '/')
	{
	  subDst = new char[dstLen + strlen(ep->d_name)];
	  subDst = strcat(strcpy(subDst, dst), ep->d_name);
	}
	else
	{
	  subDst = new char[dstLen + 1 + strlen(ep->d_name)];
	  subDst = strcat(strcat(strcpy(subDst, dst), "/"), ep->d_name);
	}
	
	cpdirtree(subDst, ep->d_name);
      }
    }
    (void) closedir(dp);
    if(chdir(cwd) < 0)
    {
      cerr << "chdir(out): " << cwd << endl;
      exit(1);
    }
    return 0;
  }
  else
  {
    cerr << "Couldn't open the directory: " << src << endl;
    exit(1);
  }
}

int main(int argc, char *argv[])
{
  // Ensure argument passed.
  if (argc < 3)
  {
    cout << "Usage: cpdirr [-d] SRC DST\n"
         << "       -d     to clear destination directory\n"
         << "       SRC    path to directory to copy from\n"
         << "       DST    path to directory to copy to";
    return 1;
  }

  char *src = argv[1];
  char *dst = argv[2];

  //Directory clearence checking
  if(strcmp(argv[1], "-d") == 0)
  {
    cleardir(argv[3]);
    src = argv[2];
    dst = argv[3];
  }

  char fullDstPath[255];
  if(dst[0] != '/')
  {
    strcat(strcat(getcwd(fullDstPath, 255),"/"), dst);
    return cpdirtree(fullDstPath, src);
  }

  return cpdirtree(dst, src);
}
