//
// Created by Rain on 10/11/2020.
//

#ifndef ARCANE_THREADSAFE_H
#define ARCANE_THREADSAFE_H

#include <mutex>
#include <functional>
#include <iostream>

namespace arcane
{
	/*template<typename T>
	class LockableResource
	{
	public:
		LockableResource (std::mutex &mutex, T *val, const bool &locked=true)
		{
			if (locked)
				m_lock = std::unique_lock<std::mutex> (mutex);
			else
				m_lock = std::unique_lock<std::mutex> (mutex, std::defer_lock);
			m_lockState = locked;

			data = val;
		}

		LockableResource (const LockableResource &lr, const bool &throwaway=false)
		{
			if (this == &lr)
				return;
			std::cout << "start" << std::endl;
			lock ();
			std::cout << "locked" << std::endl;
			lr.lock();
			std::cout << "locked 2" << std::endl;
			std::cout << data << std::endl;
			*data = *lr.data;
			std::cout << "data copied" << std::endl;
		}

		LockableResource &operator= (const T &val)
		{
			if (!m_lockState)
				lock ();

			*data = val;
			return *this;
		}

		operator T ()
		{
			if (!m_lockState)
				lock ();

			return *data;
		}

		operator T () const
		{
			if (!m_lockState)
				lock ();

			return *data;
		}

		T value () const
		{
			if (!m_lockState)
				lock ();

			return *data;
		}

		T *operator-> ()
		{
			if (!m_lockState)
				lock ();

			return data;
		}

		void lock () const
		{
			if (m_lockState)
				return;
			m_lock.lock ();
			m_lockState = true;
		}

		void unlock ()
		{
			if (!m_lockState)
				return;
			m_lock.unlock ();
			m_lockState = false;
		}

		bool locked ()
		{
			return m_lockState;
		}

		bool operator== (const LockableResource &lockableResource)
		{
			return data == lockableResource.data;
		}

		bool operator!= (const LockableResource &lockableResource)
		{
			return data != lockableResource.data;
		}

		T *data = nullptr;

	private:
		mutable std::unique_lock<std::mutex> m_lock;
		mutable bool m_lockState;
	};

	template<typename T>
	class ThreadSafe
	{
	public:
		ThreadSafe ()
		{
			m_obj = T ();
		}

		ThreadSafe (const T &obj)
		{
			m_obj = obj;
		}

		ThreadSafe (const ThreadSafe &threadSafe, const bool &throwaway=false)
		{
			std::lock_guard<std::mutex> lock (m_mutex);
			std::lock_guard<std::mutex> lock2 (threadSafe.m_mutex);
			m_obj = threadSafe.m_obj;
		}

		LockableResource<T> operator* ()
		{
			return LockableResource<T> (m_mutex, &m_obj);
		}

		LockableResource<T> getResource (const bool &lockState=true)
		{
			return LockableResource<T> (m_mutex, &m_obj, lockState);
		}

		LockableResource<T> operator () (const bool &lockState=true)
		{
			return LockableResource<T> (m_mutex, &m_obj, lockState);
		}

		T *raw ()
		{
			return m_obj;
		}

	private:
		T m_obj;
		mutable std::mutex m_mutex;
	};*/
}
#endif //ARCANE_THREADSAFE_H
