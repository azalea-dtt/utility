#include <stdio.h>

#include <time.h>

#include <sys/timeb.h>

#include <memory.h>

int main(int argc, const char * argv[])

{

    struct timeb tb; 

    struct tm t;
    char timebuf[64]; 
    memset(timebuf, 0, 64);
    

    ftime(&tb); 
    if (localtime_r(&tb.time, &t) != nullptr)
    {
        sprintf(timebuf, "%4d-%d-%d %d:%d:%d.%d \n",t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, tb.millitm);
    }

    /*t= *localtime(&tb.time);

    

    sprintf(timebuf, "%4d-%d-%d %d:%d:%d.%d \n",t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t. tm_sec, tb.millitm);
    */
    printf("timebuf--: %s", timebuf);
    return 0;

}



