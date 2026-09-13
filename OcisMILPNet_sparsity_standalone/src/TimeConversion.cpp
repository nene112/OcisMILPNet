#include "TimeConversino.h"
time_t StringToTime_t(string time)
{
  tm time_e = StringToDatetime(time);
  time_t s = mktime(&time_e);
  return s;
}
string tm_To_string(tm time)
{
  //tm to time_t
  time_t s = mktime(&time);
  return FormatTime(s);
}
//time_t  to string
string FormatTime(time_t time1)
{

  struct tm tm1;
  
  

#ifdef _MSC_VER  
  tm1 = *localtime(&time1);
#else
  localtime_r(&time1, &tm1);
#endif  
  string str = "2022-7-11 12:00:00";
  char* szTime = (char*)str.data();
  sprintf(szTime, "%d-%02d-%02d %02d:%02d",
    tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday,
    tm1.tm_hour, tm1.tm_min);
  string returnstr;
  returnstr = szTime;
  return returnstr;
}

string FormatTime_min(time_t time1)
{

  struct tm tm1;
  
  

#ifdef _MSC_VER  
  tm1 = *localtime(&time1);
#else  
  localtime_r(&time1, &tm1);
#endif  
  string str = "2022-7-11 12:00:00";
  char* szTime = (char*)str.data();
  sprintf(szTime, "%d-%02d-%02d %02d:%02d:%02d",
    tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday,
    tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
  string returnstr;
  returnstr = szTime;
  return returnstr;
}
string FormatTime_second(time_t time1)
{

  struct tm tm1;
  
  

#ifdef _MSC_VER  
  tm1 = *localtime(&time1);
#else  
  localtime_r(&time1, &tm1);
#endif  
  string str = "2022-7-11 12:00:00";
  char* szTime = (char*)str.data();
  sprintf(szTime, "%d-%02d-%02d %02d:%02d:%02d",
    tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday,
    tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
  string returnstr;
  returnstr = szTime;
  return returnstr;
}

//string to tm
tm StringToDatetime(string str)
{
  char* cha = (char*)str.data();             
  tm tm_;                                    
  int year, month, day=0, hour=0, minute=0, second = 0;
  if (str.find("-") != string::npos) {

    if (str.find("T") != string::npos) {
      sscanf(cha, "%d-%d-%dT%d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    }
    else
    {
      sscanf(cha, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    }
  }
  else if (str.find("/") != string::npos)
  {
    sscanf(cha, "%d/%d/%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
  }
  else
  {
    printf("string conversion to tm failde! string = %s\n", str.c_str());
  }


  tm_.tm_year = year - 1900;                 
  tm_.tm_mon = month - 1;                    
  tm_.tm_mday = day;                         
  tm_.tm_hour = hour;                        
  tm_.tm_min = minute;                       
  tm_.tm_sec = second;                       
  tm_.tm_isdst = -1;
  tm_.tm_wday = 0;
  tm_.tm_yday = 0;
  return tm_;                                 
}


time_t GetStartOfDay(time_t timestamp) {
    struct tm *tm_local = localtime(&timestamp);  
    if (!tm_local) return 0;  

    
    tm_local->tm_hour = 0;
    tm_local->tm_min = 0;
    tm_local->tm_sec = 0;

    return mktime(tm_local);  
}


time_t GetEndOfDay(time_t timestamp) {
    struct tm *tm_local = localtime(&timestamp);  
    if (!tm_local) return 0;  

    
    tm_local->tm_hour = 23;
    tm_local->tm_min = 59;
    tm_local->tm_sec = 59;

    return mktime(tm_local);  
}
