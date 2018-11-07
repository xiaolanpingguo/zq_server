#pragma once

#include <map>
#include <list>
#include <algorithm>
#include "interface_header/platform.h"

template < typename T >
class List
{
public:
    virtual ~List() {}
    bool                    add(const T& id);
    bool                    find(const T& id);
    bool                    remove(const T& id);
    bool                    clearAll();

    bool                    first(T& id);
    bool                    next(T& id);
    bool                    get(const int32_t index, T& id);
    int                     count();

protected:
    //idType normal;
    typedef     std::list<T>   TLISTOBJCONFIGLIST;
    TLISTOBJCONFIGLIST          mtObjConfigList;
    typename std::list<T>::iterator mCurIter;

private:
};

template < typename T >
bool List<T>::add(const T& id)
{
    //if (!find(id))
    {
        mtObjConfigList.push_back(id);
        return true;
    }

    return false;
}

template < typename T >
bool List<T>::remove(const T& id)
{
    if (find(id))
    {
        mtObjConfigList.remove(id);
        return true;
    }

    return false;
}

template < typename T >
bool List<T>::clearAll()
{
    mtObjConfigList.clear();
    return true;
}

template < typename T >
bool List<T>::first(T& id)
{
    if (mtObjConfigList.size() <= 0)
    {
        return false;
    }

    mCurIter = mtObjConfigList.begin();
    if (mCurIter != mtObjConfigList.end())
    {
        id = *mCurIter;
        return true;
    }

    return false;
}

template < typename T >
bool List<T>::next(T& id)
{
    if (mCurIter == mtObjConfigList.end())
    {
        return false;
    }

    ++mCurIter;
    if (mCurIter != mtObjConfigList.end())
    {
        id = *mCurIter;
        return true;
    }

    return false;
}

template < typename T >
bool List<T>::find(const T& id)
{
    typename TLISTOBJCONFIGLIST::iterator it = std::find(mtObjConfigList.begin(), mtObjConfigList.end(), id);
    if (it != mtObjConfigList.end())
    {
        return true;
    }

    return false;
}

template < typename T >
bool List<T>::get(const int32_t index, T& id)
{
    if (index >= mtObjConfigList.size())
    {
        return false;
    }

    typename std::list<T>::iterator it = this->mtObjConfigList.begin();
    std::advance(it, index);

    id = *it;

    return true;
}

template < typename T >
int List<T>::count()
{
    return (int)(mtObjConfigList.size());
}

