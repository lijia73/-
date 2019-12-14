#ifndef AGENDA_STORAGE_H
#define AGENDA_STORAGE_H

#include <functional>
#include <list>
#include <memory>
#include <string>
#include "Meeting.hpp"
#include "User.hpp"

class Storage {
 private:
  Storage();
  Storage(const Storage &t_another) = delete;
  void operator=(const Storage &t_another) = delete;
  bool readFromFile(void);
  bool writeToFile(void);
 public:
  static std::shared_ptr<Storage> getInstance(void);
  ~Storage();
  
  void createUser(const User &t_user);
  std::list<User> queryUser(std::function<bool(const User &)> filter) const;
  int updateUser(std::function<bool(const User &)> filter,
                 std::function<void(User &)> switcher);
  int deleteUser(std::function<bool(const User &)> filter);
  
  void createMeeting(const Meeting &t_meeting);
  std::list<Meeting> queryMeeting(
      std::function<bool(const Meeting &)> filter) const;
  int updateMeeting(std::function<bool(const Meeting &)> filter,
                    std::function<void(Meeting &)> switcher);
  int deleteMeeting(std::function<bool(const Meeting &)> filter);
  
  bool sync(void);
 private:
  static std::shared_ptr<Storage> m_instance;
  std::list<User> m_userList;
  std::list<Meeting> m_meetingList;
  bool m_dirty;
};

#endif
