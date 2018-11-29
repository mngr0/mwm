// CustomList.h

#ifndef _CUSTOMLIST_h
#define _CUSTOMLIST_h

#include <list>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifndef NULL
#define NULL 0
#endif

template <typename T>
class CustomList
{
  public:
    typedef T *iterator;

    CustomList()
    {
        _internalArray = NULL;
        _endPosition = 0;
        _allocBlocks = 0;
        _preAllocBlocks = 0;
    }

    ~CustomList()
    {
        delete[] _internalArray;
        _internalArray = NULL;
        _endPosition = 0;
        _allocBlocks = 0;
        _preAllocBlocks = 0;
    }

    CustomList(const CustomList &from)
    {
        _endPosition = from._endPosition;
        _allocBlocks = from._allocBlocks;
        _preAllocBlocks = from._preAllocBlocks;

        _internalArray = new T[_allocBlocks];

        for (int i = 0; i < _endPosition; ++i)
            _internalArray[i] = from._internalArray[i];
    }

    CustomList &operator=(const CustomList &from)
    {
        if (this != &from)
        {
            _endPosition = from._endPosition;
            _allocBlocks = from._allocBlocks;
            _preAllocBlocks = from._preAllocBlocks;

            delete[] _internalArray;
            _internalArray = NULL;

            if (_allocBlocks)
            {
                _internalArray = new T[_allocBlocks];

                for (int i = 0; i < _endPosition; ++i)
                    _internalArray[i] = from._internalArray[i];
            }
        }

        return *this;
    }

    CustomList<T> &operator=(std::list<T> from)
    {
        clear();
        while (from.size() != 0)
        {
            push_back(from.front());
            from.pop_front();
        }

        return *this;
    }

    void sort()
    {
        bool swapped = true;
        int j = 0;
        int tmp;
        while (swapped)
        {
            swapped = false;
            j++;
            for (int i = 0; i < _endPosition - j; i++)
            {
                if (_internalArray[i] > _internalArray[i + 1])
                {
                    tmp = _internalArray[i];
                    _internalArray[i] = _internalArray[i + 1];
                    _internalArray[i + 1] = tmp;
                    swapped = true;
                }
            }
        }
    }

    int indexOf(T item)
    {
        for (int i = 0; i < _endPosition; i++)
        {
            if (_internalArray[i] == item)
            {
                return i;
            }
        }
        return -1;
    }

    void push_back(T item)
    {
        if (_endPosition == _allocBlocks)
            AllocOneBlock(false);

        _internalArray[_endPosition] = item;
        ++_endPosition;
    }

    void push_front(T item)
    {
        if (_endPosition == _allocBlocks)
            AllocOneBlock(true);
        else
        {
            for (int i = _endPosition; i > 0; --i)
                _internalArray[i] = _internalArray[i - 1];
        }

        _internalArray[0] = item;
        ++_endPosition;
    }

    void remove(int index)
    {
        Serial.println(_endPosition);
        if (_endPosition == 0)
            return;

        --_endPosition;

        //if (_allocBlocks > _preAllocBlocks)
        for (int i = index; i < _endPosition; ++i)
            _internalArray[i] = _internalArray[i + 1];
        DeAllocOneBlock(false);
        Serial.println(_endPosition);
    }

    void pop_back()
    {
        if (_endPosition == 0)
            return;

        --_endPosition;

        if (_allocBlocks > _preAllocBlocks)
            DeAllocOneBlock(false);
    }

    void pop_front()
    {
        if (_endPosition == 0)
            return;

        --_endPosition;

        if (_allocBlocks > _preAllocBlocks)
            DeAllocOneBlock(true);
        else
        {
            for (int i = 0; i < _endPosition; ++i)
                _internalArray[i] = _internalArray[i + 1];
        }
    }

    iterator erase(iterator position)
    {
        int offSet = int(position - _internalArray);

        if (offSet == _endPosition - 1) // Last item.
        {
            pop_back();
            return end();
        }

        --_endPosition;

        if (_allocBlocks > _preAllocBlocks)
        {
            --_allocBlocks;
            T *newArray = new T[_allocBlocks];

            for (int i = 0; i < _endPosition; ++i)
            {
                if (i >= offSet)
                    newArray[i] = _internalArray[i + 1];
                else
                    newArray[i] = _internalArray[i];
            }

            delete[] _internalArray;
            _internalArray = newArray;
        }
        else
        {
            for (int i = offSet; i < _endPosition; ++i)
                _internalArray[i] = _internalArray[i + 1];
        }

        return _internalArray + offSet;
    }

    void reserve(int size)
    {
        if (size == 0 || size < _allocBlocks)
            return;

        _allocBlocks = size;
        _preAllocBlocks = size;

        T *newArray = new T[_allocBlocks];

        for (int i = 0; i < _endPosition; ++i)
            newArray[i] = _internalArray[i];

        delete[] _internalArray;
        _internalArray = newArray;
    }

    void clear()
    {
        if (_allocBlocks > _preAllocBlocks)
        {
            _allocBlocks = _preAllocBlocks;

            T *newArray = NULL;

            if (_allocBlocks > 0)
                newArray = new T[_allocBlocks];

            delete[] _internalArray;
            _internalArray = newArray;
        }

        _endPosition = 0;
    }

    void shrink_to_fit()
    {
        _preAllocBlocks = _endPosition;
        _allocBlocks = _endPosition;

        T *newArray = NULL;

        if (_allocBlocks > 0)
            newArray = new T[_allocBlocks];

        for (int i = 0; i < _endPosition; ++i)
            newArray[i] = _internalArray[i];

        delete[] _internalArray;
        _internalArray = newArray;
    }

    T &operator[](int x)
    {
        return _internalArray[x];
    }

    inline iterator begin() { return _internalArray; }
    inline iterator end() { return _internalArray + _endPosition; }

    inline bool empty() { return (_endPosition == 0); }
    inline unsigned int size() { return _endPosition; }
    inline unsigned int capacity() { return _allocBlocks; }

  private:
    void AllocOneBlock(bool shiftItems)
    {
        ++_allocBlocks;
        T *newArray = new T[_allocBlocks];

        for (int i = 0; i < _endPosition; ++i)
            newArray[shiftItems ? (i + 1) : i] = _internalArray[i];

        delete[] _internalArray;
        _internalArray = newArray;
    }

    void DeAllocOneBlock(bool shiftItems)
    {
        --_allocBlocks;

        if (_allocBlocks == 0)
        {
            delete[] _internalArray;
            _internalArray = NULL;
            return;
        }

        T *newArray = new T[_allocBlocks];

        for (int i = 0; i < _endPosition; ++i)
            newArray[i] = _internalArray[shiftItems ? (i + 1) : i];

        delete[] _internalArray;
        _internalArray = newArray;
    }

  private:
    T *_internalArray;
    int _endPosition;
    int _allocBlocks;
    int _preAllocBlocks;
};

#endif
