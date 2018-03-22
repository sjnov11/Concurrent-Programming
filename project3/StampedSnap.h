#ifndef __STAMPED_SNAP_H__
#define __STAMPPED_SNAP_H__

#include <cstdint>
#include <cstring>

typedef int64_t T;

class StampedSnap {
	public:
		int64_t stamp;
		T value;
		T* snap;

		StampedSnap(T value);
		StampedSnap(int64_t label, T value, T* snap);
		virtual ~StampedSnap();
};

#endif
