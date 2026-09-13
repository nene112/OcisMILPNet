#include <string>
#include <time.h>
using namespace std;

//string to tm
tm StringToDatetime(string str);
//string to time_t  require:"%d-%d-%d %d:%d:%d"
time_t StringToTime_t(string time);
//tm to string
string tm_To_string(tm time);
//time_t  to string
string FormatTime(time_t time1);

string FormatTime_min(time_t time1);
string FormatTime_second(time_t time1);

time_t GetStartOfDay(time_t timestamp);
time_t GetEndOfDay(time_t timestamp);
