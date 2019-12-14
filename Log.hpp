#include <memory>
#include <fstream>    
#include <string>    
#include <ctime>

using namespace std;
class Log{
	public:
		Log(){
			
		}
		static shared_ptr<Log> getInstance(void){
			if(instance!=NULL)
			return instance;
			else{
				instance= new shared_ptr<Storage>(new Log());
			}
		} 
		
	private:
		static shared_ptr<Log> m_instance;
};

Log::instance=NULL;
