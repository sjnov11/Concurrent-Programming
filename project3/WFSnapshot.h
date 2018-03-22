#ifndef __WF_SNAPSHOT_H__
#define __WF__SNAPSHOT_H__

#include "StampedSnap.h"

class WFSnapshot {
	private:
		StampedSnap** a_table;	// array of atomic MRSW registers
		int capacity;

		StampedSnap** collect();

	public:
		WFSnapshot(int capacity, T init);
		virtual ~WFSnapshot();

		void update(T value, int64_t tid);
		T* scan();

};

#endif




