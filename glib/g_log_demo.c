#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions

#include <vector>
#include <string>

#include <stdarg.h>

#include <getopt.h>
#include <syslog.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <glib.h>

static void log (const char *format, ...)
{
  char *str;

  va_list args;

  va_start (args, format);
  str = g_strdup_vprintf (format, args);
  va_end (args);

  printf ("Log: %s\n", str);
  

  g_free (str);

  //exit (1);
}

int main(int argc, char **argv)
{
    GTimer *clock = NULL;
    gint i;
    gdouble elapsed_time;
    gulong us; /* microseconds */
    clock = g_timer_new();
    g_timer_start(clock);
    g_print("Timer started.\n");
    g_print("Loop started.. ");
    
    while (1)
    {
        elapsed_time = g_timer_elapsed(clock, &us);
        if (elapsed_time == WAIT)
        {
            g_print("and finished.\n");
            g_timer_stop(clock);
            g_print("Timer stopped.\n");
            g_print("Elapsed: %g s\n", elapsed_time);
            g_print(" %ld us\n", us);

            break;
        }
    }
    g_timer_destroy(clock);

    log("Elapsed: %g s, %ld us\n", elapsed_time, us);
    log(" ---- %ld us\n", us);
    return 0;
}