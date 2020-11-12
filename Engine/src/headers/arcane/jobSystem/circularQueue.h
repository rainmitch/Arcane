#ifndef ARCANE_CIRCULARQUEUE_H
#define ARCANE_CIRCULARQUEUE_H

#include <cstdint>

template<typename T, size_t N>
class CircularQueue
{
public:
	CircularQueue ()
	{
		m_readPointer = 0;
		m_writePointer = 0;
		m_objects = new T[N];
	}

	~CircularQueue()
	{
		delete[] m_objects;
	}

	T *peek ()
	{
		return &m_objects[m_readPointer];
	}

	T *next ()
	{
		const size_t index = m_readPointer;
		m_readPointer = (m_readPointer + 1) % N;

		return &m_objects[index];
	}

	T *getNextFreeSlot ()
	{
		T *job = &m_objects[m_writePointer];
		m_writePointer = (m_writePointer + 1) % N;

		return job;
	}

private:
	size_t m_readPointer;
	size_t m_writePointer;
	T *m_objects;
};


#endif //ARCANE_CIRCULARQUEUE_H
