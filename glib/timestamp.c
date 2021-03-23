#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <algorithm>
#include <syslog.h>
#include <sstream> 

#include <sys/timeb.h>

std::string CurrentDateTime()
{
    struct timeb tb; // <sys/timeb.h> 
    struct tm tstruct; 
    std::ostringstream oss; 
    char buf[128]; 
    ftime(&tb); 
    if (nullptr != localtime_r(&tb.time, &tstruct))
    { 
        strftime(buf, sizeof(buf), "%Y-%m-%d %T", &tstruct); 
        oss << buf; 
        oss << tb.millitm; 
    } 
    //std::cout << oss.str() << std::endl; oss.str(""); 
    return oss.str();
}

int main(int argc, char **argv)
{
	std::cout << CurrentDateTime() << std::endl;
	return 0;
}