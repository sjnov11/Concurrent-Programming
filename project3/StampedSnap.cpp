#include "StampedSnap.h"

extern int64_t number_of_threads;


StampedSnap::StampedSnap(T value) {
	this->stamp = 0;
	this->value = value;
	this->snap = new T[number_of_threads]();
}

StampedSnap::StampedSnap(int64_t label, T value, T* snap) {
	this->stamp = label;
	this->value = value;
	this->snap = new T[number_of_threads]();
	memcpy(this->snap, snap, sizeof(*this->snap) * number_of_threads);
}

StampedSnap::~StampedSnap() {
	delete[] this->snap;
}
