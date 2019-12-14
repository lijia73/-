#include <iostream>
#include <sstream>
#include "Date.hpp"

using namespace std;

string standard(int i)
{
    stringstream ss;
    if(i<10)
    {
        ss<<"0"<<i;
    }
    else ss<<i;

    return ss.str();
}
bool leapyear(int y)
{
    if((y%4==0)&&(y%100!=0)||(y%400==0)) return true;
    return false;
}
int DayofMonth(int y,int m)
{
    int monthArray[13]= {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if(leapyear(y)&&m==2)
    {
        return 29;
    }
    return monthArray[m];
}

/**
   * @brief default constructor
   */
Date::Date()
{
    m_year=m_month=m_day=m_hour=m_minute=0;
}

/**
* @brief constructor with arguments
*/
Date::Date(int t_year, int t_month, int t_day, int t_hour, int t_minute):m_year(t_year),m_month(t_month),m_day(t_day),m_hour(t_hour),m_minute(t_minute)
{

}

/**
* @brief constructor with a string
*/
Date::Date(const std::string &dateString)
{
    *this=stringToDate(dateString);
}
/**
* @brief return the year of a Date
* @return   a integer indicate the year of a date
*/
int Date::getYear(void) const
{
    return m_year;
}

/**
* @brief set the year of a date
* @param a integer indicate the new year of a date
*/
void Date::setYear(const int t_year)
{
    m_year=t_year;
}

/**
* @brief return the month of a Date
* @return   a integer indicate the month of a date
*/
int Date::getMonth(void) const
{
    return m_month;
}

/**
* @brief set the month of a date
* @param a integer indicate the new month of a date
*/
void Date::setMonth(const int t_month)
{
    m_month=t_month;
}

/**
* @brief return the day of a Date
* @return   a integer indicate the day of a date
*/
int Date::getDay(void) const
{
    return m_day;
}

/**
* @brief set the day of a date
* @param a integer indicate the new day of a date
*/
void Date::setDay(const int t_day)
{
    m_day=t_day;
}

/**
* @brief return the hour of a Date
* @return   a integer indicate the hour of a date
*/
int Date::getHour(void) const
{
    return m_hour;
}

/**
* @brief set the hour of a date
* @param a integer indicate the new hour of a date
*/
void Date::setHour(const int t_hour)
{
    m_hour=t_hour;
}

/**
* @brief return the minute of a Date
* @return   a integer indicate the minute of a date
*/
int Date::getMinute(void) const
{
    return m_minute;
}

/**
* @brief set the minute of a date
* @param a integer indicate the new minute of a date
*/
void Date::setMinute(const int t_minute)
{
    m_minute=t_minute;
}

/**
*   @brief check whether the date is valid or not
*   @return the bool indicate valid or not
*/
bool Date::isValid(const Date &t_date)
{
    if(!(t_date.getYear()>=1000&&t_date.getYear()<=9999)) return false;
    else
    {
        if(!(t_date.getMonth()>=1&&t_date.getMonth()<=12)) return false;
        else
        {
            if(t_date.getDay()<=0||t_date.getDay()>DayofMonth(t_date.getYear(),t_date.getMonth()))return false;
            else
            {
                if(t_date.getHour()<0||t_date.getHour()>23) return false;
                    else
                    {
                        if(t_date.getMinute()<0||t_date.getMinute()>59) return false;
                    }
            }
        }
    }

    return true;

}

/**
* @brief convert a string to date, if the format is not correct return
* 0000-00-00/00:00
* @return a date
*/
Date Date::stringToDate(const std::string &t_dateString)
{
    int flag=0;
    if(t_dateString.size()!=16) flag=1;
    for(int i=0; i<t_dateString.size(); i++)
    {
        if(i==4||i==7||i==10||i==13)
        {
            continue;
        }
        else
        {
            if(t_dateString[i]<'0'||t_dateString[i]>'9')
            {
                flag=1;
                break;
            }
        }
    }
    if(t_dateString[4]!='-'||t_dateString[7]!='-'||t_dateString[10]!='/'||t_dateString[13]!=':')flag=1;
    if(flag==1) return Date();

    stringstream ss;
    int y,m,d,h,mi;
    char t;
    ss<<t_dateString;
    ss>>y>>t>>m>>t>>d>>t>>h>>t>>mi;
    return Date(y,m,d,h,mi);
}

/**
* @brief convert a date to string, if the date is invalid return
* 0000-00-00/00:00
*/
string Date::dateToString(const Date &t_date)
{
    if(!isValid(t_date)) return "0000-00-00/00:00";
    stringstream ss;
    ss<<t_date.getYear()<<"-"<<standard(t_date.getMonth())<<"-"<<standard(t_date.getDay())<<"/"<<standard(t_date.getHour())<<":"<<standard(t_date.getMinute());
    return ss.str();
}

/**
*  @brief overload the assign operator
*/
Date& Date::operator=(const Date &t_date)
{
    m_year=t_date.getYear();
    m_month=t_date.getMonth();
    m_day=t_date.getDay();
    m_hour=t_date.getHour();
    m_minute=t_date.getMinute();
    return *this;
}

/**
* @brief check whether the CurrentDate is equal to the t_date
*/
bool Date::operator==(const Date &t_date) const
{
    if(m_year==t_date.getYear())
    {
        if(m_month==t_date.getMonth())
        {
            if(m_day==t_date.getDay())
            {
                if(m_hour==t_date.getHour())
                {
                    if(m_minute==t_date.getMinute())
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

/**
* @brief check whether the CurrentDate is  greater than the t_date
*/
bool Date::operator>(const Date &t_date) const
{
    if(m_year>t_date.getYear())return true;
    else if(m_year==t_date.getYear())
    {
        if(m_month>t_date.getMonth()) return true;
        else if(m_month==t_date.getMonth())
        {
            if(m_day>t_date.getDay()) return true;
            else if(m_day==t_date.getDay())
            {
                if(m_hour>t_date.getHour())return true;
                else if(m_hour==t_date.getHour())
                {
                    if(m_minute>t_date.getMinute()) return true;
                }
            }
        }
    }
    return false;
}

/**
* @brief check whether the CurrentDate is  less than the t_date
*/
bool Date::operator<(const Date &t_date) const
{
    return !(*this>t_date||*this==t_date);

}

/**
* @brief check whether the CurrentDate is  greater or equal than the t_date
*/
bool Date::operator>=(const Date &t_date) const
{
    return (*this>t_date||*this==t_date);
}

/**
* @brief check whether the CurrentDate is  less than or equal to the t_date
*/
bool Date::operator<=(const Date &t_date) const
{
    return (*this<t_date||*this==t_date);
}
