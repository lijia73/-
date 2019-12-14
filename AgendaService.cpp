#include "AgendaService.hpp"
#include "Exception.hpp"
using namespace std;
//std::shared_ptr<Storage> m_storage;
/**
   * constructor
   */
AgendaService::AgendaService(){
	startAgenda();
}

  /**
   * destructor
   */
AgendaService::~AgendaService(){
	quitAgenda();
}

  /**
   * check if the username match password
   * @param userName the username want to login
   * @param password the password user enter
   * @return if success, true will be returned
   */
bool AgendaService::userLogIn(const std::string &userName, const std::string &password){
	list<User> users=listAllUsers();
	for(auto it=users.begin(); it!=users.end(); it++)
    {
        if(it->getName()==userName&&it->getPassword()==password){      	
        	return true;
		}
    }
    return false;
} 

  /**
   * regist a user
   * @param userName new user's username
   * @param password new user's password
   * @param email new user's email
   * @param phone new user's phone
   * @return if success, true will be returned
   */
bool AgendaService::userRegister(const std::string &userName, const std::string &password,
                    const std::string &email, const std::string &phone){
    list<User> users=listAllUsers();
	for(auto it=users.begin(); it!=users.end(); it++)
    {
        if(it->getName()==userName){
        	return false;
		}
    }          	
    m_storage->createUser(User(userName,password,email,phone));
	return true;               	
}

  /**
   * delete a user
   * @param userName user's username
   * @param password user's password
   * @return if success, true will be returned
   */
bool AgendaService::deleteUser(const std::string &userName, const std::string &password){
	deleteAllMeetings(userName);
	auto filter=[userName, password](const User &user) {
        if(userName==user.getName()&&password==user.getPassword()){
        	return true;
		}
		else{
			return false;
		}
    };
	
     auto parlist=listAllParticipateMeetings(userName);
     for(auto it=parlist.begin();it!=parlist.end();it++){
     	quitMeeting(userName, it->getTitle());
	 } 
			
	int num=m_storage->deleteUser(filter);

	if(num>0) return true;
	return false;
}

  /**
   * list all users from storage
   * @return a user list result
   */
std::list<User> AgendaService::listAllUsers(void) const{
	return m_storage->queryUser([](const User &)->bool{return true;});
} 

  /**
   * create a meeting
   * @param userName the sponsor's userName
   * @param title the meeting's title
   * @param participator the meeting's participator
   * @param startData the meeting's start date
   * @param endData the meeting's end date
   * @return if success, true will be returned
   */
bool AgendaService::createMeeting(const std::string &userName, const std::string &title,
                     const std::string &startDate, const std::string &endDate,
                     const std::vector<std::string> &participator){	
                     
    if(participator.empty()) return false;
	//is validDate
	Date start(startDate);
	Date end(endDate);
	if(!Date::isValid(start)||!Date::isValid(end)||start>=end) throw Exception("Date is invalid.");

	//only title overrall
	auto filter=[&](const Meeting& meeting){
		if(meeting.getTitle()==title) return true;
		else return false;
	};
	list<Meeting> temp=m_storage->queryMeeting(filter);
	if(!temp.empty()) throw Exception("The title has been used.");
	
	//register judge
	vector<string> all_people = participator;
	all_people.push_back(userName);
	
	for(auto it=all_people.begin();it!=all_people.end();it++){
		if(m_storage->queryUser([&](const User &user){
			if(user.getName()==*it) return true;
			return false;
		}).size()!=1){
			if(it==all_people.end()-1) throw Exception("Sponsor did not registered.");
			else throw Exception("Some participators did not registered.");
		}
	}

	for(auto it=participator.begin();it!=participator.end();it++){
		if((*it)==userName)
		throw Exception("Sponsor can not be in participator list.");
	}
	
	for(int i=0;i<participator.size();i++){
		for(int j=0;j<participator.size();j++){
			if(i!=j&&participator[i]==participator[j]) throw Exception("Duplicate participators.");
		}
	}
	//overlap judge
	for(int i=0;i<all_people.size();i++){
		list<Meeting> m = m_storage->queryMeeting([&](const Meeting& meeting){
			if(meeting.getSponsor()==all_people[i]||meeting.isParticipator(all_people[i])){
				if(!(start>=meeting.getEndDate()||end<=meeting.getStartDate())){
					return true;
				}
			}
			return false;
		});
		
		if(!m.empty()) throw Exception("Someone is busy.");
	}
	
	Meeting m(userName, participator,start,end,title);
  	 m_storage->createMeeting(m);	
	return true;
}

  /**
   * add a participator to a meeting
   * @param userName the sponsor's userName
   * @param title the meeting's title
   * @param participator the meeting's participator
   * @return if success, true will be returned
   */
bool AgendaService::addMeetingParticipator(const std::string &userName,
                              const std::string &title,
                              const std::string &participator){ 
    
    if(userName==participator) return false;
	//participator register
    list<User> u=m_storage->queryUser([participator](const User& user) {
	    if(user.getName()==participator){
	           return true;
	        }
	        return false;
    });
    
    if(u.size()!=1){
   		return false;
    }
	
	//overloap judge							                  	
	auto filter1=[&](const Meeting& meeting){
		if(meeting.getSponsor()==userName&&meeting.getTitle()==title&&!meeting.isParticipator(participator)){
			Date start=meeting.getStartDate();
			Date end=meeting.getEndDate();
			
			list<Meeting> overloapmeeting=m_storage->queryMeeting([&](const Meeting&meeting1){
				if(meeting1.getSponsor()==participator||meeting1.isParticipator(participator)){
					if(end<=meeting1.getStartDate()||start>=meeting1.getEndDate()){
						return false;
					}
					else return true;
				}
				return false;
			});
			if(overloapmeeting.empty())return true;
		}
		return false;
	}; 
	
	auto switcher=[participator](Meeting &meeting) {
        meeting.addParticipator(participator);
    };   
	
	int num=m_storage->updateMeeting(filter1, switcher);
	
    return num==1;                     	
}

  /**
   * remove a participator from a meeting
   * @param userName the sponsor's userName
   * @param title the meeting's title
   * @param participator the meeting's participator
   * @return if success, true will be returned
   */
bool AgendaService::removeMeetingParticipator(const std::string &userName,const std::string &title,const std::string &participator){
	 if(userName==participator)	return false;
 
	list<User>u1=m_storage->queryUser([userName](const User &u){
		if(u.getName()==userName){
			return true;
		}
		else{
			return false;
		}
	});
	list<User>u2=m_storage->queryUser([participator](const User &u){
		if(u.getName()==participator){
			return true;
		}
		else{
			return false;
		}
	});
	
	if(u1.empty()||u2.empty()){
		return false;
	}
	
	auto filter = [userName, title](const Meeting &meeting) {
        if (meeting.getTitle()==title&&meeting.getSponsor()==userName){
        	return true;
		}
        else{
        	return false;
		}
    };
    list<Meeting> target=m_storage->queryMeeting(filter);
    
    if(target.size()!=1){
    	return false;
	}
	
	Meeting ftarget=*(target.begin());
	if(!ftarget.isParticipator(participator)){
		return false;
	}
    auto switcher=[participator](Meeting &meeting) {
    	meeting.removeParticipator(participator);
    };
    
    int num=m_storage->updateMeeting(filter, switcher);
    
    m_storage->deleteMeeting([](const Meeting &meeting){
    	return meeting.getParticipator().empty();
	});
	
    return num>0;
}

  /**
   * quit from a meeting
   * @param userName the current userName. need to be the participator (a sponsor can not quit his/her meeting)
   * @param title the meeting's title
   * @return if success, true will be returned
   */
bool AgendaService::quitMeeting(const std::string &userName, const std::string &title){	
	auto filter=[userName,title](const Meeting &meeting){
     	if(meeting.isParticipator(userName)&&meeting.getTitle()==title){
     		return true;
	 	}
	 	else{
	 		return false;
	 	}
	};
	auto switcher=[userName](Meeting &meeting) {
        meeting.removeParticipator(userName);
    };
    
    int num=m_storage->updateMeeting(filter, switcher);

    m_storage->deleteMeeting([](const Meeting &meeting) {
        if(meeting.getParticipator().empty()){
        	return true;
		}
		else{
			return false;
		}
    });
    
    return num>0;

}
  /**
   * search a meeting by username and title
   * @param userName as a sponsor OR a participator
   * @param title the meeting's title
   * @return a meeting list result
   */
std::list<Meeting> AgendaService::meetingQuery(const std::string &userName,
                                  const std::string &title) const{
	auto filter=[userName,title](const Meeting & meeting){
		if((meeting.isParticipator(userName)||meeting.getSponsor()==userName)&&meeting.getTitle()==title){
			return true;
		}
		else return false;
	};
	
	return m_storage->queryMeeting(filter);	                                  	
}
  /**
   * search a meeting by username, time interval
   * @param userName as a sponsor OR a participator
   * @param startDate time interval's start date
   * @param endDate time interval's end date
   * @return a meeting list result
   */
std::list<Meeting> AgendaService::meetingQuery(const std::string &userName,
                                  const std::string &startDate,
                                  const std::string &endDate) const{
    list<Meeting> empty;
    Date start(startDate);
	Date end(endDate);
    if(!Date::isValid(start)||!Date::isValid(end)||start>end) return empty;
    
	auto filter=[userName,start,end](const Meeting & meeting){
		if(meeting.isParticipator(userName)||meeting.getSponsor()==userName){
			if(meeting.getStartDate()<=end&&meeting.getEndDate()>=start)
			return true;
		}
		else return false;
	};
	
	return m_storage->queryMeeting(filter);	                                    	
}

  /**
   * list all meetings the user take part in
   * @param userName user's username
   * @return a meeting list result
   */
list<Meeting> AgendaService::listAllMeetings(const std::string &userName) const{
	auto filter=[userName](const Meeting & meeting){
		if(meeting.isParticipator(userName)||meeting.getSponsor()==userName){
			return true;
		}
		else return false;
	};
	
	return m_storage->queryMeeting(filter);	      	
}

  /**
   * list all meetings the user sponsor
   * @param userName user's username
   * @return a meeting list result
   */
std::list<Meeting> AgendaService::listAllSponsorMeetings(const std::string &userName) const{
	auto filter=[userName](const Meeting & meeting){
		if(meeting.getSponsor()==userName){
			return true;
		}
		else return false;
	};
	
	return m_storage->queryMeeting(filter);	 	
}

  /**
   * list all meetings the user take part in and sponsor by other
   * @param userName user's username
   * @return a meeting list result
   */
std::list<Meeting> AgendaService::listAllParticipateMeetings(const std::string &userName) const{
	auto filter=[userName](const Meeting & meeting){
		if(meeting.isParticipator(userName)){
			return true;
		}
		else return false;
	};
	
	return m_storage->queryMeeting(filter);	 
}

  /**
   * delete a meeting by title and its sponsor
   * @param userName sponsor's username
   * @param title meeting's title
   * @return if success, true will be returned
   */
bool AgendaService::deleteMeeting(const std::string &userName, const std::string &title){
	auto filter=[userName,title](const Meeting & meeting){
		if(meeting.getSponsor()==userName&&meeting.getTitle()==title){
			return true;
		}
		else return false;
	};
	int num=m_storage->deleteMeeting(filter);
	
	if(num>0) {
		return true;
	}
	else{
		return false;
	} 
}

  /**
   * delete all meetings by sponsor
   * @param userName sponsor's username
   * @return if success, true will be returned
   */
bool AgendaService::deleteAllMeetings(const std::string &userName){
	auto filter=[userName](const Meeting & meeting){
		if(meeting.getSponsor()==userName){
			return true;
		}
		else return false;
	};
	int num=m_storage->deleteMeeting(filter);
	
	if(num>0) {
		return true;
	}
	else{
		return false;
	} 	
}

  /**
   * start Agenda service and connect to storage
   */
void AgendaService::startAgenda(void){
  	m_storage=Storage::getInstance();
}

  /**
   * quit Agenda service
   */
void AgendaService::quitAgenda(void){
	m_storage->sync();
}
