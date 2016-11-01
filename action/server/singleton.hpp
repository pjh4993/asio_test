#ifndef SINGLETON
#define SINGLETON
#include <mutex>
#include <atomic>
template <typename T>
class TemplateSingleton{
	protected:
		TemplateSingleton(){

		}
		virtual ~TemplateSingleton(){

		}
		static std::mutex mtx;
	public:
		static T* getInstance(){
			std::lock_guard<std::mutex> guard(mtx);
			if(m_pInstance == NULL)
				m_pInstance = new T;
			return m_pInstance;
		};
		static void DestroyInstance(){
			std::lock_guard<std::mutex> guard(mtx);
			if(m_pInstance){
				delete m_pInstance;
				m_pInstance = NULL;
			}
		};
	private:
		static std::atomic<T> * m_pInstance;
};
#endif
