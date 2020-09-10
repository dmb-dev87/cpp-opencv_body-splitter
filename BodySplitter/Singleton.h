/*
* Singleton.h
*
*  Created on: Apr 22, 2013 - yep its not a lie
*      Author: rob
*/

#pragma once

#include <assert.h>

template<typename T>
class Singleton
{
protected:
	static T* ms_singleton;
public:
	Singleton()
	{
		assert(!ms_singleton);
		ms_singleton = static_cast<T*>(this);
	}

	~Singleton()
	{
		assert(ms_singleton);
		ms_singleton = 0;
	}

	static T& GetSingleton()
	{
		assert(ms_singleton);
		return *ms_singleton;
	}
	static T& getSingleton()
	{
		assert(ms_singleton);
		return *ms_singleton;
	}

	static T* GetSingletonPtr()
	{
		assert(ms_singleton);
		return ms_singleton;
	}
	static T* getSingletonPtr()
	{
		assert(ms_singleton);
		return ms_singleton;
	}


};
template <typename T> T* Singleton <T>::ms_singleton = nullptr;