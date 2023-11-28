// by Mark Featherstone (C) 2019 All Rights Reserved.
#ifndef SINGLETON_H
#define SINGLETON_H

/*
We often want only one instance of an object, especially if we are creating manager classes
to hold onto groups of objects and manage their lifetime. A singleton is a "design pattern"
designed to do just that - ensure there is only ever one instance of a specific class.

As we'll use it again and again, we don't want to repeat ourselves, this template and the
two macros help us write less code to get the job done.
Example - declaration
---------------------
	#include "Singleton.h"
	class ModeManager : public Singleton<ModeManager>
	{
	public:
		//add all your functions and data
	};

Example - in use
----------------
	new ModeManager;
	....
	ModeManager::Get()->DoStuff();
	....
	delete GetModeManager();

*/

template<class T> class Singleton
{
public:
	Singleton()
	{
		assert( spSingleton == nullptr);
		spSingleton = static_cast<T*>(this);
	}

	virtual ~Singleton()
	{
		assert( spSingleton != nullptr);
		spSingleton = nullptr;
	}

	static T& Get()	{ 
		assert(spSingleton!=nullptr);
		return *spSingleton; 
	}

private:
	static T* spSingleton;
	Singleton(Singleton const&) = delete;
	void operator=(Singleton const&) = delete;
};

template<typename T> T* Singleton<T>::spSingleton = 0;

#endif
