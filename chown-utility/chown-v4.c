/*
  VERSION NO.4
  DESCRIPTION: This version of CHOWN handle single file/directory etc , multiple space separated
  files/directories/links etc and recursive implementation of CHOWN on single directory
  USAGE: Here's the list of cases which handle this version

        case 1:  ./chown-v?.c user:group filename or space separated filenames/
        space seprated directories or directory  (To change user and group)
                
        case 2:  ./chown-v?.c user       filename or space separated filenames/
        space seprated directories or directory  (To change user only)
                
        case 3:  ./chown-v?.c     :group filename or space separated filenames/
        space seprated directories or directory  (To change group only)
                
        case 4:  ./chown-v?.c     :      filename or space separated filenames/
        space seprated directories or directory  (Nothing will happen)

  OPTION: -R (for recursive implementation of CHOWN on signle directory or multiple directories)
            
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>

void doChown(const char *filepath, uid_t uid, gid_t gid);
void doChownOnDir(const char *filepath, uid_t, gid_t gid);
int isExist(char *source);
gid_t getGID(const char *group_name);
uid_t getUID(const char *user_name);
char getType(int mode_t);
char **tokenize(char *arg);
char mypath[1024];
int IS_ONLY_GROUP = 0;
int main(int argc, char *argv[])
{
        int isRecursive = 0;
        char *option = NULL;
        char *source = NULL;
        char *userName = NULL;
        char *groupName = NULL;
        // Checking recurive option for multiple files and directories
        for (int i = 3; i < argc; i++)
        {
                if (strcmp(argv[i], "-R") == 0)
                {
                        isRecursive = 1;
                        break;
                }
        }
        if (argc == 1 || argc == 2)
        {
                if (argc == 1)
                        fprintf(stderr, "chown: missing operand\nTry 'chown --help' for more information.\n");
                if (argc == 2)
                        fprintf(stderr, "chown: missing operand after '%s'\nTry 'chown --help' for more information.\n", argv[1]);
                exit(0);
        }
        else if (argc == 3)
        {
                source = argv[2];
                // checking file exist
                if (isExist(source) == 0)
                {
                        char **infoToken = {NULL};
                        if (strcmp(argv[1], ":") != 0)
                        {
                                infoToken = tokenize(argv[1]);
                                userName = infoToken[0];
                                groupName = infoToken[1];
                        }
                        if (IS_ONLY_GROUP == 1 && infoToken[0] != NULL)
                        {
                                printf("%d", IS_ONLY_GROUP);
                                groupName = userName;
                                userName = NULL;
                        }
                        // Handling cases
                        // visit: https://www.computerhope.com/unix/uchown.htm
                        if (userName != NULL && groupName == NULL)
                        {
                                // case1 "user:" (change only user)
                                uid_t uid = getUID(userName);
                                doChown(source, uid, -1);
                        }
                        else if (userName == NULL && groupName != NULL)
                        {
                                // case2 ":group" (change only group)
                                gid_t gid = getGID(groupName);
                                doChown(source, -1, gid);
                        }
                        else if (userName == NULL && groupName == NULL)
                        {
                                // case3 ":" (Do nothing)
                                doChown(source, -1, -1);
                        }
                        else
                        {
                                // case4 "user:group" (change user and group)
                                uid_t uid = getUID(userName);
                                gid_t gid = getGID(groupName);
                                doChown(source, uid, gid);
                        }
                }
                else
                {
                        fprintf(stderr, "chown: cannot access '%s': No such file or directory", source);
                        exit(0);
                }
        }
        else if (argc == 4 & strcmp(argv[3], "-R") == 0)
        {
                source = argv[2];
                struct stat tempstat;
                if (isExist(source) == 0)
                        stat(source, &tempstat);
                // checking file exist
                if (isExist(source) == 0 && getType(tempstat.st_mode) == 'd')
                {
                        char **infoToken = {NULL};
                        if (strcmp(argv[1], ":") != 0)
                        {
                                infoToken = tokenize(argv[1]);
                                userName = infoToken[0];
                                groupName = infoToken[1];
                        }
                        if (IS_ONLY_GROUP == 1 && infoToken[0] != NULL)
                        {
                                printf("%d", IS_ONLY_GROUP);
                                groupName = userName;
                                userName = NULL;
                        }
                        // Handling cases
                        // visit: https://www.computerhope.com/unix/uchown.htm
                        if (userName != NULL && groupName == NULL)
                        {
                                // case1 "user:" (change only user)
                                uid_t uid = getUID(userName);
                                doChownOnDir(source, uid, -1);
                        }
                        else if (userName == NULL && groupName != NULL)
                        {
                                // case2 ":group" (change only group)
                                gid_t gid = getGID(groupName);
                                doChownOnDir(source, -1, gid);
                        }
                        else if (userName == NULL && groupName == NULL)
                        {
                                // case3 ":" (Do nothing)
                                doChownOnDir(source, -1, -1);
                        }
                        else
                        {
                                // case4 "user:group" (change user and group)
                                uid_t uid = getUID(userName);
                                gid_t gid = getGID(groupName);
                                doChownOnDir(source, uid, gid);
                        }
                }
                else if (isExist(source) == 0)
                {
                        char **infoToken = {NULL};
                        if (strcmp(argv[1], ":") != 0)
                        {
                                infoToken = tokenize(argv[1]);
                                userName = infoToken[0];
                                groupName = infoToken[1];
                        }
                        if (IS_ONLY_GROUP == 1 && infoToken[0] != NULL)
                        {
                                printf("%d", IS_ONLY_GROUP);
                                groupName = userName;
                                userName = NULL;
                        }
                        // Handling cases
                        // visit: https://www.computerhope.com/unix/uchown.htm
                        if (userName != NULL && groupName == NULL)
                        {
                                // case1 "user:" (change only user)
                                uid_t uid = getUID(userName);
                                doChown(source, uid, -1);
                        }
                        else if (userName == NULL && groupName != NULL)
                        {
                                // case2 ":group" (change only group)
                                gid_t gid = getGID(groupName);
                                doChown(source, -1, gid);
                        }
                        else if (userName == NULL && groupName == NULL)
                        {
                                // case3 ":" (Do nothing)
                                doChown(source, -1, -1);
                        }
                        else
                        {
                                // case4 "user:group" (change user and group)
                                uid_t uid = getUID(userName);
                                gid_t gid = getGID(groupName);
                                doChown(source, uid, gid);
                        }
                }
                else
                {
                        fprintf(stderr, "chown: cannot access '%s': No such file or directory", source);
                        exit(0);
                }
        }
        else if (argc >= 4 && isRecursive == 0)
        {
                for (int i = 2; i < argc; i++)
                {
                        source = argv[i];
                        // checking file exist
                        if (isExist(source) == 0)
                        {
                                char **infoToken = {NULL};
                                if (strcmp(argv[1], ":") != 0)
                                {
                                        infoToken = tokenize(argv[1]);
                                        userName = infoToken[0];
                                        groupName = infoToken[1];
                                }
                                if (IS_ONLY_GROUP == 1 && infoToken[0] != NULL)
                                {
                                        printf("%d", IS_ONLY_GROUP);
                                        groupName = userName;
                                        userName = NULL;
                                }
                                // Handling cases
                                // visit: https://www.computerhope.com/unix/uchown.htm
                                if (userName != NULL && groupName == NULL)
                                {
                                        // case1 "user:" (change only user)
                                        uid_t uid = getUID(userName);
                                        doChown(source, uid, -1);
                                }
                                else if (userName == NULL && groupName != NULL)
                                {
                                        // case2 ":group" (change only group)
                                        gid_t gid = getGID(groupName);
                                        doChown(source, -1, gid);
                                }
                                else if (userName == NULL && groupName == NULL)
                                {
                                        // case3 ":" (Do nothing)
                                        doChown(source, -1, -1);
                                }
                                else
                                {
                                        // case4 "user:group" (change user and group)
                                        uid_t uid = getUID(userName);
                                        gid_t gid = getGID(groupName);
                                        doChown(source, uid, gid);
                                }
                        }
                        else
                        {
                                fprintf(stderr, "chown: cannot access '%s': No such file or directory", source);
                                exit(0);
                        }
                }
        }
        else if (argc >= 4 && isRecursive == 1)
        {
                for (int i = 2; i < argc - 1; i++)
                {
                        source = argv[i];
                        struct stat tempstat;
                        if (isExist(source) == 0)
                                stat(source, &tempstat);
                        // checking file exist
                        if (isExist(source) == 0 && getType(tempstat.st_mode) == 'd')
                        {
                                char **infoToken = {NULL};
                                if (strcmp(argv[1], ":") != 0)
                                {
                                        infoToken = tokenize(argv[1]);
                                        userName = infoToken[0];
                                        groupName = infoToken[1];
                                }
                                if (IS_ONLY_GROUP == 1 && infoToken[0] != NULL)
                                {
                                        printf("%d", IS_ONLY_GROUP);
                                        groupName = userName;
                                        userName = NULL;
                                }
                                // Handling cases
                                // visit: https://www.computerhope.com/unix/uchown.htm
                                if (userName != NULL && groupName == NULL)
                                {
                                        // case1 "user:" (change only user)
                                        uid_t uid = getUID(userName);
                                        doChownOnDir(source, uid, -1);
                                }
                                else if (userName == NULL && groupName != NULL)
                                {
                                        // case2 ":group" (change only group)
                                        gid_t gid = getGID(groupName);
                                        doChownOnDir(source, -1, gid);
                                }
                                else if (userName == NULL && groupName == NULL)
                                {
                                        // case3 ":" (Do nothing)
                                        doChownOnDir(source, -1, -1);
                                }
                                else
                                {
                                        // case4 "user:group" (change user and group)
                                        uid_t uid = getUID(userName);
                                        gid_t gid = getGID(groupName);
                                        doChownOnDir(source, uid, gid);
                                }
                        }
                        else if (isExist(source) == 0)
                        {
                                char **infoToken = {NULL};
                                if (strcmp(argv[1], ":") != 0)
                                {
                                        infoToken = tokenize(argv[1]);
                                        userName = infoToken[0];
                                        groupName = infoToken[1];
                                }
                                if (IS_ONLY_GROUP == 1 && infoToken[0] != NULL)
                                {
                                        printf("%d", IS_ONLY_GROUP);
                                        groupName = userName;
                                        userName = NULL;
                                }
                                // Handling cases
                                // visit: https://www.computerhope.com/unix/uchown.htm
                                if (userName != NULL && groupName == NULL)
                                {
                                        // case1 "user:" (change only user)
                                        uid_t uid = getUID(userName);
                                        doChown(source, uid, -1);
                                }
                                else if (userName == NULL && groupName != NULL)
                                {
                                        // case2 ":group" (change only group)
                                        gid_t gid = getGID(groupName);
                                        doChown(source, -1, gid);
                                }
                                else if (userName == NULL && groupName == NULL)
                                {
                                        // case3 ":" (Do nothing)
                                        doChown(source, -1, -1);
                                }
                                else
                                {
                                        // case4 "user:group" (change user and group)
                                        uid_t uid = getUID(userName);
                                        gid_t gid = getGID(groupName);
                                        doChown(source, uid, gid);
                                }
                        }
                        else
                        {
                                fprintf(stderr, "chown: cannot access '%s': No such file or directory", source);
                                exit(0);
                        }
                }
        }
}
char getType(int mode_t)
{
        if (S_ISDIR(mode_t))
                return 'd';
        else if (S_ISREG(mode_t))
                return '-';
        else if (S_ISLNK(mode_t))
                return 'l';
        else if (S_ISCHR(mode_t))
                return 'c';
        else if (S_ISBLK(mode_t))
                return 'b';
        else if (S_ISSOCK(mode_t))
                return 's';
        else if (S_ISFIFO(mode_t))
                return 'p';
        else
                return '?';
}
void doChown(const char *file_path, uid_t uid, gid_t gid)
{
        if (chown(file_path, uid, gid) == -1)
        {
                fprintf(stderr, "chown: changing ownership of '%s': Operation not permitted\n", file_path);
                exit(0);
        }
}
uid_t getUID(const char *user_name)
{
        struct passwd *pwd;
        pwd = getpwnam(user_name);
        if (pwd == NULL)
        {
                fprintf(stderr, "chown: failed to get uid against user '%s'\n", user_name);
                exit(0);
        }
        return pwd->pw_uid;
}
gid_t getGID(const char *group_name)
{
        struct group *grp;
        grp = getgrnam(group_name);
        if (grp == NULL)
        {
                fprintf(stderr, "chown: failed to get gid aginst group '%s'\n", group_name);
                exit(0);
        }
        return grp->gr_gid;
}
int isExist(char *source)
{
        struct stat st = {0};
        int rv = 0;
        if (stat(source, &st) == -1)
        {
                rv = -1;
        }
        return rv;
}
char **tokenize(char *arg)
{
        //allocate memory
        char **arglist = (char **)malloc(sizeof(char *) * (3 + 1));
        for (int j = 0; j < 3 + 1; j++)
        {
                arglist[j] = (char *)malloc(sizeof(char) * 12);
                bzero(arglist[j], 12);
        }
        if (arg[0] == '\0') //if user has entered nothing and pressed enter key
                return 0;
        int argnum = 0; //slots used
        char *cp = arg; //pos in string
        char *start;
        int len;
        // Check for 'is group only' as second argument
        if (cp[0] == ':')
                IS_ONLY_GROUP = 1;

        while (*cp != '\0')
        {

                while (*cp == ':' || *cp == '\t' || *cp == ' ') //skip leading spaces
                        cp++;
                start = cp; //start of the username
                len = 1;
                //find the end of agrgument or groupname
                while (*++cp != '\0' && !(*cp == ' ' || *cp == '\t' || *cp == ':'))
                        len++;
                strncpy(arglist[argnum], start, len);
                arglist[argnum][len] = '\0';
                argnum++;
        }
        arglist[argnum] = NULL;
        return arglist;
}

void doChownOnDir(const char *filepath, uid_t uid, gid_t gid)
{
        DIR *dir;
        struct dirent *entry;
        if (!(dir = opendir(filepath)))
                return;
        doChown(filepath, uid, gid); // on directory
        while ((entry = readdir(dir)) != NULL)
        {
                if (entry->d_type == DT_DIR)
                {
                        char path[1024];
                        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                                continue;
                        snprintf(path, sizeof(path), "%s/%s", filepath, entry->d_name);
                        doChownOnDir(path, uid, gid);
                }
                else
                {
                        snprintf(mypath, sizeof(mypath), "%s/%s", filepath, entry->d_name);
                        doChown(mypath, uid, gid); // on directory/ies content
                }
        }
        closedir(dir);
}