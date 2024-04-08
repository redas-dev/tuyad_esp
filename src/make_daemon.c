#include "make_daemon.h"

extern int make_daemon(int flags)
{
    int maxfd, fd;

    switch(fork())
    {
        case -1: goto error;
        case 0: break;
        default: _exit(EXIT_SUCCESS);
    }

    if(setsid() == -1)
        goto error;
    
    switch(fork())
    {
        case -1: goto error;
        case 0: break;
        default: _exit(EXIT_SUCCESS);
    }

    if(!(flags & NO_UMASK0)) 
        umask(0);

    if(!(flags & NO_CHDIR)) 
        chdir("/");

    if(!(flags & NO_CLOSE_FILES))
    {
        maxfd = sysconf(_SC_OPEN_MAX);
        if(maxfd == -1) 
            maxfd = MAX_CLOSE;

        for(fd = 0; fd < maxfd; fd++)
            close(fd);
    }

    if(!(flags & NO_REOPEN_STD_FDS))
    {
        close(STDIN_FILENO);

        fd = open("/dev/null", O_RDWR);
        if(fd != STDIN_FILENO) goto error;
        if(dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO) goto error;
        if(dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO) goto error;
    }

    syslog(LOG_INFO, "%s", "Daemon initialized.");

    return 0;
    error:
        syslog(LOG_ERR, "%s", "Failed to make a daemon.");
        return -1;
}