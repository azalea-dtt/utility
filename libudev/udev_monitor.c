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
#include <getopt.h>


#include <libudev.h>

#define DURATION 10000
#define WAIT    3
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
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
/*int main(int argc, char **argv)
{
    GTimer *clock = NULL;
    gint i;
    gdouble elapsed_time;
    gulong us; 
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
}*/

void print_device(struct udev_device* dev)
{
    const char* action = udev_device_get_action(dev);
    if (! action)
        action = "exists";
    const char* path = udev_device_get_devpath(dev);
    if (!path) path = "no_path";
    const char* subsystem = udev_device_get_subsystem(dev);
    if (!subsystem) subsystem = "no_sub";
    printf("subsystem[%s], devtype[%s], action[%6s], path[%s]\n",
           udev_device_get_subsystem(dev),
           udev_device_get_devtype(dev),
           action,
           path);
}
static int test_monitor(struct udev *udev)
{
	struct udev_monitor *udev_monitor = NULL;
	int fd_ep;
	int fd_udev = -1;
	struct epoll_event ep_udev, ep_stdin;
	fd_ep = epoll_create1(EPOLL_CLOEXEC);
	if (fd_ep < 0) {
		printf("error creating epoll fd: %m\n");
		goto out;
	}
	udev_monitor = udev_monitor_new_from_netlink(udev, "kernel"); //other option: udev
	if (udev_monitor == NULL) {
		printf("no socket\n");
		goto out;
	}
	fd_udev = udev_monitor_get_fd(udev_monitor);
	if (udev_monitor_filter_add_match_subsystem_devtype(udev_monitor, "block", NULL) < 0 ||
	    udev_monitor_filter_add_match_subsystem_devtype(udev_monitor, "tty", NULL) < 0 ||
        udev_monitor_filter_add_match_subsystem_devtype(udev_monitor, "typec", NULL) < 0 ||
	    udev_monitor_filter_add_match_subsystem_devtype(udev_monitor, "usb", "usb_device") < 0) {
		printf("filter failed\n");
		goto out;
	}
	if (udev_monitor_enable_receiving(udev_monitor) < 0) {
		printf("bind failed\n");
		goto out;
	}
	memset(&ep_udev, 0, sizeof(struct epoll_event));
	ep_udev.events = EPOLLIN; //event for reading
	ep_udev.data.fd = fd_udev;
	if (epoll_ctl(fd_ep, EPOLL_CTL_ADD, fd_udev, &ep_udev) < 0) {
		printf("fail to add fd to epoll: %m\n");
		goto out;
	}
//monitor stdin for keyboard event
	memset(&ep_stdin, 0, sizeof(struct epoll_event));
	ep_stdin.events = EPOLLIN;
	ep_stdin.data.fd = STDIN_FILENO;
	if (epoll_ctl(fd_ep, EPOLL_CTL_ADD, STDIN_FILENO, &ep_stdin) < 0) {
		printf("fail to add fd to epoll: %m\n");
		goto out;
	}
	for (;;) {
		int fdcount;
		struct epoll_event ev[4];
		struct udev_device *device;
		int i;
		printf("waiting for events from udev, press ENTER to exit\n");
		fdcount = epoll_wait(fd_ep, ev, ARRAY_SIZE(ev), -1);
		printf("epoll fd count: %i\n", fdcount);
		for (i = 0; i < fdcount; i++) {
			if (ev[i].data.fd == fd_udev && ev[i].events & EPOLLIN) {
				device = udev_monitor_receive_device(udev_monitor);
				if (device == NULL) {
					printf("no device from socket\n");
					continue;
				}
				print_device(device);
				udev_device_unref(device);
			} else if (ev[i].data.fd == STDIN_FILENO && ev[i].events & EPOLLIN) {
				printf("exiting loop\n");
				goto out;
			}
		}
	}
out:
	if (fd_ep >= 0)
		close(fd_ep);
	udev_monitor_unref(udev_monitor);
	return 0;
}
int main()
{
    struct udev *udev = NULL;
	udev = udev_new();
	log("context: %p\n", udev);
	if (udev == NULL) {
		log("no context\n");
		return 1;
	}
	test_monitor(udev);
	return 0;
}
