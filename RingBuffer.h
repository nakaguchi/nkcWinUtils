#pragma once

#include <mutex>

template<class T>
class RingBuffer
{
	static const int DEEFAULT_SIZE = 4;
	T* _data;
	std::mutex* _mtxData;
	std::mutex _mtxIndex;
	int _size;
	int _write;
	int _read;
	bool _update;
public:
	// �R���X�g���N�^
	// size �o�b�t�@�̃T�C�Y
	// initial �S�v�f�̏����l
	RingBuffer(int size = DEEFAULT_SIZE);
	RingBuffer(int size, T initial);

	// �f�X�g���N�^
	~RingBuffer();

	// �f�[�^���i�[����
	void Put(T value);

	// �ŐV�̃f�[�^���o��
	// peeking true ���o���L�^���Ȃ�  false (�f�t�H���g�j���o���L�^����
	// past �ߋ��ɂ����̂ڂ�i�f�t�H���g=0�j
	T Get(bool peeking = false, int past = 0);

	// �V�����f�[�^���ǉ����ꂽ��
	bool IsNew() { return _update; };
};

template<class T>
RingBuffer<T>::RingBuffer(int size)
	: _data(NULL)
	, _write(0)
	, _read(0)
	, _update(false)
{
	_size = size;
	_data = new T[_size];
	_mtxData = new std::mutex[_size];
}

template<class T>
RingBuffer<T>::RingBuffer(int size, T initial)
	: _data(NULL)
	, _write(0)
	, _read(0)
	, _update(false) {
	_size = size;
	_data = new T[_size];
	for (int i = 0; i < _size; i++) _data[i] = initial;
	_mtxData = new std::mutex[_size];
}

template<class T>
RingBuffer<T>::~RingBuffer() {
	delete[] _data;
	delete[] _mtxData;
}

template<class T>
void RingBuffer<T>::Put(T value) {
	int write = _write;
	_mtxData[write].lock();
	_data[write] = value;
	_mtxData[write].unlock();

	_mtxIndex.lock();
	_read = write;
	_write = (write + 1) % _size;
	_update = true;
	_mtxIndex.unlock();
}

template<class T>
T RingBuffer<T>::Get(bool peeking, int past) {
	int read = (_read + _size - past) % _size;
	_mtxData[read].lock();
	T value = _data[read];
	_mtxData[read].unlock();

	if (!peeking) {
		_mtxIndex.lock();
		_update = false;
		_mtxIndex.unlock();
	}

	return value;
}
