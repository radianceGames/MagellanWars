#include "universe.h"
#include "planet.h"
#include "tech.h"
#include "project.h"
#include "component.h"
#include "ship.h"
#include "action.h"
#include "relation.h"
#include "defense.plan.h"
#include "message.h"
#include "empire.h"
#include "event.h"
#include "magistrate.h"
#include "fortress.h"
#include "blackmarket.h"
#include "spy.h"
#include "rank.h"

#ifndef __ARCHSPACE_PORTAL_H__
#define __ARCHSPACE_PORTAL_H__

#define DEFAULT_USER_LEVEL    UL_PLAYER
#define DEFAULT_DEV_LEVEL     DL_NONE
#define MIN_USER_LEVEL        UL_BANNED
#define MAX_USER_LEVEL        UL_ADMIN
#define MIN_DEV_LEVEL         DL_NONE
#define MAX_DEV_LEVEL         DL_ALL
extern TZone gPortalUserZone;

// Generic User Levels                
enum EUserLevel {
     UL_BANNED = -1,
     UL_PLAYER = 0,
     UL_MODERATOR = -3,
     UL_ADMIN = 1,
     UL_DEVELOPER = -2
 };
       
//  Developer User Levels
enum EDevLevel {
     DL_NONE = 0,
     DL_BALANCE,
     DL_CREATIVE,
     DL_IMAGE,
     DL_ALL
 };
      
char *user_level_to_string(EUserLevel aUserLevel);
char *dev_level_to_string(EDevLevel aDevLevel);
EUserLevel string_to_user_level(char *str);
EDevLevel  string_to_dev_level(char *str);

class CPortalUser: public CStore
{
      public:
      CPortalUser(MYSQL_ROW aRow);
      virtual ~CPortalUser();
      
      virtual const char *table() { return "asbb_users"; }
      virtual CString& query();

      protected:
      enum EStoreFlag
      {
           STORE_USER_LEVEL = 0,
           STORE_DEV_LEVEL,
           STORE_NAME,
           STORE_ID,
           STORE_PASSWORD,
           STORE_EMAIL,
           STORE_AGE,
           STORE_COUNTRY,
           STORE_FIRST_LOGIN,
           STORE_IP           
      };      
      
      private:
      int mPortalID;
      EUserLevel mUserLevel;
      EDevLevel mDevLevel;
      CString mName;
      CString mPassword;
      CString mEmail;
      int mAge;
      CString mCountry; 
      CString mFirstLogin;
      CString mIP;
      CPortalUser *mPrev;
      CPortalUser *mNext;
      
      public: // get
      inline int get_portal_id() const;
      inline EUserLevel get_user_level() const;
      inline EDevLevel get_dev_level() const;
      inline const char* get_name() const;
      inline const char* get_password() const;
      inline const char* get_email() const;
      inline int get_age() const;
      inline const char* get_country() const;
      inline const char* get_first_login() const;
      inline const char* get_ip() const;

      public: // set
      inline void set_portal_id(int aPortalID);
      inline void set_user_level(EUserLevel aUserLevel);
      inline void set_dev_level(EDevLevel aDevLevel);
      inline bool set_name(char *aName);
      inline bool set_password(char *aPassword);
      inline bool set_email(char *aEmail);
      inline void set_age(int aAge);
      inline bool set_country(char *aCountry);
      inline bool set_first_login(char *aLogin);
      inline bool set_ip(char *aIP);
      protected:
      RECYCLE(gPortalUserZone);
};

inline int
CPortalUser::get_portal_id() const
{
    return mPortalID;
}

inline EUserLevel
CPortalUser::get_user_level() const
{
    return mUserLevel;
}

inline EDevLevel
CPortalUser::get_dev_level() const
{
    return mDevLevel;
}

inline const char*
CPortalUser::get_name() const
{
    return (char *)mName;
}

inline const char*
CPortalUser::get_password() const
{
    return (char *)mPassword;
}

inline const char*
CPortalUser::get_email() const
{
    return (char *)mEmail;
}

inline const char*
CPortalUser::get_country() const
{
    return (char *)mCountry;
}

inline const char*
CPortalUser::get_first_login() const
{
    return (char *)mFirstLogin;
}

inline const char*
CPortalUser::get_ip() const
{
    return (char *)mIP;
}

inline int
CPortalUser::get_age() const
{
    return mAge;
}

inline void
CPortalUser::set_portal_id(int aPortalID)
{
    mPortalID = aPortalID;
    mStoreFlag += STORE_ID;    
}

inline void
CPortalUser::set_user_level(EUserLevel aUserLevel)
{
    mUserLevel = aUserLevel;
    mStoreFlag += STORE_USER_LEVEL;    
}

inline void
CPortalUser::set_dev_level(EDevLevel aDevLevel)
{
    mDevLevel = aDevLevel;
    mStoreFlag += STORE_DEV_LEVEL;    
}

inline void
CPortalUser::set_age(int aAge)
{
    mAge = aAge;
    mStoreFlag += STORE_AGE;    
}

inline bool
CPortalUser::set_name(char *aName)
{
    if (aName == NULL)
       return false;
    mName = aName;
    mStoreFlag += STORE_NAME;    
    return true;
}

inline bool
CPortalUser::set_password(char *aPassword)
{
    if (aPassword == NULL)
       return false;
    mPassword = aPassword;
    mStoreFlag += STORE_PASSWORD;    
    return true;
}

inline bool
CPortalUser::set_email(char *aEmail)
{
    if (aEmail == NULL)
       return false;
    mEmail = aEmail;
    mStoreFlag += STORE_EMAIL;
    return true;
}

inline bool
CPortalUser::set_country(char *aCountry)
{
    if (aCountry == NULL)
       return false;
    mCountry = aCountry;
    mStoreFlag += STORE_COUNTRY;    
    return true;
}

inline bool
CPortalUser::set_first_login(char *aFirstLogin)
{
    if (aFirstLogin == NULL)
       return false;
    mFirstLogin = aFirstLogin;
    mStoreFlag += STORE_FIRST_LOGIN;
    return true;
}

inline bool
CPortalUser::set_ip(char *aIP)
{
    if (aIP == NULL)
       return false;
    mIP = aIP;
    mStoreFlag += STORE_IP;
    return true;
}


extern TZone gPortalUserListZone;
class CPortalUserList: public CSortedList
{
      public:
      CPortalUserList();
      virtual ~CPortalUserList();
      bool add_user(CPortalUser* aPortalUser);
      bool remove_user(int aPortalID);
      CPortalUser *get_by_portal_id(int aPortalID);
	
      protected:
      virtual bool free_item(TSomething aItem);
      virtual int compare(TSomething aItem1, TSomething aItem2) const;
      virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
      virtual const char *debug_info() const { return "portal user list"; }      
      RECYCLE(gPortalUserListZone);
};

class CPortalUserTable: public CSortedList
{
      public:
      CPortalUserTable();
      virtual ~CPortalUserTable();
      
      bool load(CMySQL &aMySQL);
      bool load_new_by_id(int aPortalID);
            
      int add_user(CPortalUser* aPortalUser);
      bool remove_user(int aPortalID);
      CPortalUser* get_by_portal_id(int aPortalID);
      
      protected:
      virtual bool free_item(TSomething aItem);
      virtual int compare(TSomething aItem1, TSomething aItem2) const;
      virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
      virtual const char *debug_info() const { return "portal user table"; }      
};
#endif
