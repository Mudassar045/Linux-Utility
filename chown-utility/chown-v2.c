#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>

void doChown(const char *filepath, uid_t uid, gid_t gid);
int isExist(char *source);
gid_t getGID(const char *group_filepath);
uid_t getUID(const char *user_filepath);
char getType(mode_t);
char **tokenize(char *arg);
void doChownOnDir(const char *filepath,uid_t uid, gid_t gid);
char mypath[1024];
int main(int argc, char *argv[])
{
        char *option;
        char *source;
        char *userfilepath;
        char *groupfilepath;

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
                        char **infoToken = tokenize(argv[1]);
                        userfilepath = infoToken[0];
                        groupfilepath = infoToken[1];
                        // Handling cases
                        // visit: https://www.computerhope.com/unix/uchown.htm
                        if (userfilepath != NULL && groupfilepath == NULL)
                        {
                                // case1 "user:" (change only user)
                                uid_t uid = getUID(userfilepath);
                                doChown(source, uid, -1);
                        }
                        else if (userfilepath == NULL && groupfilepath != NULL)
                        {
                                // case2 ":group" (change only group)
                                gid_t gid = getGID(groupfilepath);
                                doChown(source, -1, gid);
                        }
                        else if (userfilepath == NULL && groupfilepath == NULL)
                        {
                                // case3 ":" (Do nothing)
                                doChown(source, -1, -1);
                        }
                        else
                        {
                                // case4 "user:group" (change user and group)
                                uid_t uid = getUID(userfilepath);
                                gid_t gid = getGID(groupfilepath);
                                doChown(source, uid, gid);
                        }
                }
                else
                {
                        fprintf(stderr, "chown: cannot access '%s': No such file or directory", source);
                        exit(0);
                }
        }
        else if (argc == 4)
        {
                source = argv[2];
                option = argv[3];
                struct stat tempstat;
                if(isExist(source)==0)
                   stat(source,&tempstat);
                // checking file exist
                if (isExist(source) == 0 && strcmp(option,"-R")==0 && getType(tempstat.st_mode)=='d')
                {
                        char **infoToken = tokenize(argv[1]);
                        userfilepath = infoToken[0];
                        groupfilepath = infoToken[1];
                        // Handling cases
                        // visit: https://www.computerhope.com/unix/uchown.htm
                        if (userfilepath != NULL && groupfilepath == NULL)
                        {
                                // case1 "user:" (change only user)
                                uid_t uid = getUID(userfilepath);
                                doChownOnDir(source, uid, -1);
                        }
                        else if (userfilepath == NULL && groupfilepath != NULL)
                        {
                                // case2 ":group" (change only group)
                                gid_t gid = getGID(groupfilepath);
                                doChownOnDir(source, -1, gid);
                        }
                        else if (userfilepath == NULL && groupfilepath == NULL)
                        {
                                // case3 ":" (Do nothing)
                                doChownOnDir(source, -1, -1);
                        }
                        else
                        {
                                // case4 "user:group" (change user and group)
                                uid_t uid = getUID(userfilepath);
                                gid_t gid = getGID(groupfilepath);
                                doChownOnDir(source, uid, gid);
                        }
                }
                else
                {
                        fprintf(stderr, "chown: cannot access '%s': No such file or directory", source);
                        exit(0);
                }
        }
}
char getType(mode_t mode)
{
        if (S_ISDIR(mode))
                return 'd';
        else if (S_ISREG(mode))
                return '-';
        else if (S_ISLNK(mode))
                return 'l';
        else if (S_ISCHR(mode))
                return 'c';
        else if (S_ISBLK(mode))
                return 'b';
        else if (S_ISSOCK(mode))
                return 's';
        else if (S_ISFIFO(mode))
                return 'p';
        else
                return '?';
}
void doChown(const char *file_path, uid_t uid, gid_t gid)
{
        if (chown(file_path, uid, gid) == -1)
        {
                fprintf(stderr, "chown: changing ownership of '%s': Operation not permitted\n",file_path);
                exit(0);
        }
} 
uid_t getUID(const char *user_filepath)
{
        struct passwd *pwd;
        pwd = getpwnam(user_filepath);
        if (pwd == NULL)
        {
                fprintf(stderr, "chown: failed to get uid against user '%s'\n", user_filepath);
                exit(0);
        }
        return pwd->pw_uid;
}
gid_t getGID(const char *group_filepath)
{
        struct group *grp;
        grp = getgrnam(group_filepath);
        if (grp == NULL)
        {
                fprintf(stderr, "chown: failed to get gid aginst group '%s'\n", group_filepath);
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
        while (*cp != '\0')
        {
                while (*cp == ':' || *cp == '\t' || *cp == ' ') //skip leading spaces
                        cp++;
                start = cp; //start of the userfilepath
                len = 1;
                //find the end of agrgument or groupfilepath
                while (*++cp != '\0' && !(*cp == ' ' || *cp == '\t' || *cp == ':'))
                        len++;
                strncpy(arglist[argnum], start, len);
                arglist[argnum][len] = '\0';
                argnum++;
        }
        arglist[argnum] = NULL;
        return arglist;
}
void doChownOnDir(const char *filepath,uid_t uid, gid_t gid)
{
    DIR *dir;
    struct dirent *entry;
    if (!(dir = opendir(filepath)))
        return;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", filepath, entry->d_name);
            doChown(path,uid,gid);
            doChownOnDir(path,uid,gid);
        } else {
            snprintf(mypath, sizeof(mypath), "%s/%s", filepath, entry->d_name);
            doChown(mypath,uid,gid);
        }
    }
    closedir(dir);
}