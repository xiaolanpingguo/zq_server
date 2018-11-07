#pragma once 


namespace zq {

template <class T>
class Singleton
{
public:

	Singleton()
	{
		m_pInstance = static_cast<T*>(this);
	}

	~Singleton()
	{
		m_pInstance = nullptr;
	}

public:

	static T* get_instance_ptr()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new T;
		}

		return m_pInstance;
	}

	static T& get_instance()
	{
		return *get_instance_ptr();
	}

	static void release_instance()
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}

private:
	static T* m_pInstance;
};

template <class T>
T* Singleton<T>::m_pInstance = nullptr;

}