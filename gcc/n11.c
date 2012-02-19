#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int isdir(const char *path)
{
  struct stat st_buf;
  char buf[FILENAME_MAX];

  /* Get the status of the file system object. */
  if (stat(path, &st_buf) != 0)
  {
    fprintf(stderr, "Error: %s, %s cwd: %s\n", 
	    strerror(errno), path, getcwd(buf, sizeof(buf)));
    exit(1);
  }
  return S_ISDIR(st_buf.st_mode);
}

int rm(const char *path)
{
  if(isdir(path))
  {
    char cwd[FILENAME_MAX];
    DIR *dp = opendir(path);
    struct dirent *ep;

    getcwd(cwd, sizeof(cwd));
    if(dp != NULL)
    {
      if(chdir(path) < 0)
      {
	fprintf(stderr, "chdir(in): %s\n", path);
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
	fprintf(stderr, "chdir(out): %s\n", cwd);
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
  char buf[FILENAME_MAX];

  /* Get the status of the file system object. */
  if (stat(dirpath, &st_buf) != 0)
  {
    fprintf(stderr, "Error: %s, %s cwd: %s\n", 
	    strerror(errno), dirpath, getcwd(buf, sizeof(buf)));
    exit(1);
  }
  if(S_ISDIR(st_buf.st_mode))
  {
    rm(dirpath);
    if(mkdir(dirpath, st_buf.st_mode) != 0)
    {
      fprintf(stderr, "Error: %s, %s cwd: %s\n", 
	      strerror(errno), dirpath, getcwd(buf, sizeof(buf)));
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
  /* Get the current directory to know where to return in the end. */
  char cwd[FILENAME_MAX];

  DIR *dp;
  struct dirent *ep;
  char *subDst;
  int dstLen = strlen(dst);
  char absoluteSrc[FILENAME_MAX];

  struct stat st_buf;

  getcwd(cwd, sizeof(cwd));
  printf("startwd: %s\n", cwd);

  dp = opendir(src);
  if(dp != NULL)
  {
    if(chdir(src) < 0)
    {
      fprintf(stderr, "chdir(in): %s\n", src);
      exit(1);
    }
    while((ep = readdir(dp)) != NULL)
    {
      if((strcmp(ep->d_name, ".") == 0) || (strcmp(ep->d_name, "..") == 0))
	continue;

      /* Get the status of the file system object. */
      if (stat(ep->d_name, &st_buf) != 0)
      {
	fprintf(stderr, "Error: %s, %s cwd: %s\n", 
		strerror(errno), ep->d_name, getcwd(cwd, sizeof(cwd)));
	exit(1);
      }

      if(S_ISDIR(st_buf.st_mode))
      {
	getcwd(absoluteSrc, sizeof(absoluteSrc));

	printf("dirs: %s/%s\n", absoluteSrc, ep->d_name);

	if(chdir(dst) < 0)
	{
	  fprintf(stderr, "chdir(indst): %s\n", dst);
	  exit(1);
	}

	/* Checking for existance */
	if((mkdir(ep->d_name, st_buf.st_mode) != 0) && (errno != EEXIST))
	{
	  fprintf(stderr, "Error: %s, %s cwd: %s\n", 
		  strerror(errno), ep->d_name, getcwd(cwd, sizeof(cwd)));
	  exit(1);
	}
	if(chdir(absoluteSrc) < 0)
	{
	  fprintf(stderr, "chdir(outdst): %s\n", absoluteSrc);
	  exit(1);
	}

	if(dst[dstLen-1] == '/')
	{
	  subDst = (char *) malloc(sizeof(char)*(dstLen + strlen(ep->d_name)));
	  subDst = strcat(strcpy(subDst, dst), ep->d_name);
	}
	else
	{
	  subDst = (char *) malloc(sizeof(char)*(dstLen + 1 + strlen(ep->d_name)));
	  subDst = strcat(strcat(strcpy(subDst, dst), "/"), ep->d_name);
	}

	cpdirtree(subDst, ep->d_name);

	free(subDst);
      }
    }
    (void) closedir(dp);
    if(chdir(cwd) < 0)
    {
      fprintf(stderr, "chdir(out): %s\n", cwd);
      exit(1);
    }
    return 0;
  }
  else
  {
    fprintf(stderr, "Couldn't open the directory: %s\n", src);
    exit(1);
  }
}

int main(int argc, char *argv[])
{
  char *src;
  char *dst;
  char fullDstPath[FILENAME_MAX];

  /* Ensure necessary arguments passed. */
  if (argc < 3)
  {
    printf("%s%s%s%s",
	   "Usage: cpdirr [-d] SRC DST\n",
	   "-d     to clear destination directory\n",
	   "SRC    path to directory to copy from\n",
	   "DST    path to directory to copy to\n");
    return 1;
  }

  /* Directory clearence checking */
  if(strcmp(argv[1], "-d") == 0)
  {
    if(argc < 4)
    {
      printf("%s%s%s%s",
	     "Usage: cpdirr [-d] SRC DST\n",
	     "-d     to clear destination directory\n",
	     "SRC    path to directory to copy from\n",
	     "DST    path to directory to copy to\n");
      return 1;
    }
    if(!isdir(argv[3]))
    {
      fprintf(stderr, "Target: %s: Isn't a directory.\n", argv[3]);
      return 1;
    }

    cleardir(argv[3]);
    src = argv[2];
    dst = argv[3];
  }
  else
  {
    src = argv[1];
    dst = argv[2];
  }

  if(!isdir(src))
  {
    fprintf(stderr, "Source: %s: Isn't a directory.\n", src);
    return 1;
  }
  if(!isdir(dst))
  {
    fprintf(stderr, "Target: %s: Isn't a directory.\n", dst);
    return 1;
  }

  if(dst[0] != '/')
  {
    strcat(strcat(getcwd(fullDstPath, sizeof(fullDstPath)),"/"), dst);
    return cpdirtree(fullDstPath, src);
  }

  return cpdirtree(dst, src);
}
