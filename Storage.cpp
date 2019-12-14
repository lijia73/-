#include <iostream>
#include "Storage.hpp"
#include "Path.hpp"
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

shared_ptr<Storage> Storage::m_instance = NULL;

Storage::Storage()
{
    m_dirty=false;
    readFromFile();
}


/**
*   read file content into memory
*   @return if success, true will be returned
*/
bool Storage::readFromFile(void)
{

    ifstream user(Path::userPath);
    if(!(user.is_open())) return false;
    ifstream meeting(Path::meetingPath);
    if(!(meeting.is_open())) return false;

    string ustr;
    while(getline(user,ustr))
    {
    	//¡°name¡±,¡°123¡±,¡°123@123.com¡±,¡°123456¡±
    	if(ustr=="") return false;
        vector<string> v;
        istringstream iss(ustr);
		string token;
		while(getline(iss,token,',')){
			v.push_back(token);
		}
		
		for(auto i:v){ 
			i=i.substr(1,i.length()-2);
		}
		
		string name=v[0];
		string password=v[1];
		string email=v[2];
		string phone=v[3];
        User u(name,password,email,phone);
        m_userList.push_back(u);
    }

    user.close();

    string mstr;
    while(getline(meeting,mstr))
    { // ¡±sponsor¡±,¡±participators¡±,¡±startDate¡±,¡±endDate¡±,¡±title¡±
    // ¡°A¡±,¡°B&C¡±,¡°2016-08-01/00:00¡±,¡°2016-08-01/12:00¡±,¡±meeting¡°
        string sponsor,participators,startDate,endDate,title;
        if(mstr=="") return false;
		
		vector<string> v;
        istringstream iss(mstr);
		string token;
		while(getline(iss,token,',')){
			v.push_back(token);
		}
       
		for(auto i:v){ 
			i=i.substr(1,i.length()-2);
		}
		
		sponsor=v[0];
		participators=v[1];
		startDate=v[2];
		endDate=v[3];
		title=v[4];
		
        vector<string> pa;
        string part;
        stringstream ss(participators);
        while(getline(ss,part,'&')){
        	pa.push_back(part);
		}
        Date startTime(startDate),endTime(endDate);
        Meeting meeting(sponsor,pa,startTime,endTime,title);
        m_meetingList.push_back(meeting);

    }

    meeting.close();
    return true;
}

/**
*   write file content from memory
*   @return if success, true will be returned
*/
bool Storage::writeToFile(void)
{
    ofstream user(Path::userPath);
    if(!(user.is_open())) return false;
    ofstream meeting(Path::meetingPath);
    if(!(meeting.is_open())) return false;

    for(auto it=m_userList.begin(); it!=m_userList.end(); it++)
    {
        user<<'"'<<(*it).getName()<<'"'<<','<<'"'<<(*it).getPassword()<<'"'<<','<<'"'<<(*it).getEmail()<<'"'<<','<<'"'<<(*it).getPhone()<<'"';
        user<<'\n';
    }
    user.close();

    for(auto it=m_meetingList.begin(); it!=m_meetingList.end(); it++)
    {
        meeting<<'"'<<it->getSponsor()<<'"'<<','<<'"';
        vector<string> temp=(*it).getParticipator();
        for(int i=0; i<temp.size()-1; i++)
        {
            meeting<<temp[i]<<'&';
        }
        meeting<<temp[temp.size()-1];

        meeting<<'"'<<','<<'"'<<Date::dateToString((*it).getStartDate())<<'"'<<','<<'"'<<Date::dateToString((*it).getEndDate())<<'"'<<','<<'"'<<(*it).getTitle()<<'"';
        meeting<<'\n';
    }
    meeting.close();
    return true;
}


/**
* get Instance of storage
* @return the pointer of the instance
*/
std::shared_ptr<Storage> Storage::getInstance(void)
{
    if(m_instance == NULL) m_instance = shared_ptr<Storage>(new Storage())
    ;
    return m_instance;
}

/**
*   destructor
*/
Storage::~Storage()
{
    sync();
}

// CRUD for User & Meeting
// using C++11 Function Template and Lambda Expressions

/**
* create a user
* @param a user object
*/
void Storage::createUser(const User &t_user)
{
    m_userList.push_back(t_user);
    m_dirty=true;
}

/**
* query users
* @param a lambda function as the filter
* @return a list of fitted users
*/
std::list<User> Storage::queryUser(std::function<bool(const User &)> filter) const
{
    list<User> out;
    for(auto it=m_userList.begin(); it!=m_userList.end(); it++)
    {
        if(filter(*it)) out.push_back(*it);
    }
    return out;
}

/**
* update users
* @param a lambda function as the filter
* @param a lambda function as the method to update the user
* @return the number of updated users
*/
int Storage::updateUser(std::function<bool(const User &)> filter,
                        std::function<void(User &)> switcher)
{
    int count=0;
    for(auto it=m_userList.begin(); it!=m_userList.end(); it++)
    {
        if(filter(*it))
        {
            switcher(*it);
            count++;
        }
    }
    if(count>0) m_dirty=true;
    return count;
}

/**
* delete users
* @param a lambda function as the filter
* @return the number of deleted users
*/
int Storage::deleteUser(std::function<bool(const User &)> filter)
{
    int count=0;
    for(auto it=m_userList.begin(); it!=m_userList.end();)
    {
        if(filter(*it))
        {
            it=m_userList.erase(it);
            count++;
        }
        else it++;
    }

    if(count>0) m_dirty=true;
    return count;
}

/**
* create a meeting
* @param a meeting object
*/
void Storage::createMeeting(const Meeting &t_meeting)
{
    m_meetingList.push_back(t_meeting);
    m_dirty=true;
}

/**
* query meetings
* @param a lambda function as the filter
* @return a list of fitted meetings
*/
std::list<Meeting> Storage::queryMeeting(std::function<bool(const Meeting &)> filter) const
{
    list<Meeting> out;
    for(auto it=m_meetingList.begin(); it!=m_meetingList.end(); it++)
    {
        if(filter(*it)) out.push_back(*it);
    }
    return out;
}

/**
* update meetings
* @param a lambda function as the filter
* @param a lambda function as the method to update the meeting
* @return the number of updated meetings
*/
int Storage::updateMeeting(std::function<bool(const Meeting &)> filter,
                           std::function<void(Meeting &)> switcher)
{
    int count=0;
    list<Meeting>::iterator it;
    for(it=m_meetingList.begin(); it!=m_meetingList.end(); it++)
    {
        if(filter(*it))
        {
            switcher(*it);
            count++;
        }
    }
    if(count>0) m_dirty=true;
    return count;
}

/**
* delete meetings
* @param a lambda function as the filter
* @return the number of deleted meetings
*/
int Storage::deleteMeeting(std::function<bool(const Meeting &)> filter)
{
    int count=0;
    for(auto it=m_meetingList.begin(); it!=m_meetingList.end(); )
    {
        if(filter(*it))
        {
            it=m_meetingList.erase(it);
            count++;
        }
        else it++;
    }

    if(count>0) m_dirty=true;
    return count;
}

/**
* sync with the file
*/
bool Storage::sync(void)
{
	if(m_dirty==true){
		m_dirty=false;
   		return writeToFile();
	}
    
}
