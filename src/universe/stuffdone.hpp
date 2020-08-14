#ifndef STUFF_DONE_HPP
#define STUFF_DONE_HPP

#include <map>

class StuffDone
{
public:
    typedef int type;
    typedef std::pair<int,int> Key;
    typedef std::map<Key,type> SDFMap;

    type& sdf(int x, int y)
    {
        return sdf(Key(x,y));
    }
    type& sdf(const Key& key)
    {
        return getIterator(key)->second;
    }
    void writeTo(std::ostream& file) const
    {
        for(auto& entry : stuffDoneFlags_)
            file << "SDF " << entry.first.first << ' ' << entry.first.second << ' ' << entry.second << '\n';
    }
    void clear() {stuffDoneFlags_.clear();}
    SDFMap::iterator getIterator(const Key& key)
    {
        auto it = stuffDoneFlags_.find(key);
	if(it != stuffDoneFlags_.end()) return it;
        stuffDoneFlags_[key] = 0;
	return stuffDoneFlags_.find(key);
    }
private:
    SDFMap stuffDoneFlags_;
};

class PointersMap
{
public:
    StuffDone::type getPointer(int pointer)
    {
        if(pointer < 10) return 0;
        if(pointer < 100) return magicPointers_[pointer-10];
        auto it = pointers_.find(pointer);
        if(it == pointers_.end()) return 0;
        return it->second->second;
    }
    void setPointer(int pointer, int x, int y, StuffDone& stuffDone)
    {
        pointers_[pointer] = stuffDone.getIterator(StuffDone::Key(x,y));
    }
    void clearPointer(int pointer)
    {
        pointers_.erase(pointer);
    }
    void setMagicPointer(int pointer, int value)
    {
        magicPointers_[pointer-10] = value;
    }
    void loadPointer(int pointer, int x, int y, StuffDone& stuffDone)
    {
        if(pointer < 10) return;
        if(pointer < 100) setMagicPointer(pointer,x);
        else setPointer(pointer,x,y,stuffDone);
    }
    void writePointers(std::ostream& file) const
    {
        for(int i = 0 ; i < magicPointers_.size() ; i++)
            file << "POINTER " << i << ' ' << magicPointers_[i] << '\n';
        for(auto& pointer : pointers_)
            file << "POINTER " << pointer.first << ' ' << pointer.second->first.first << ' ' << pointer.second->first.second << '\n';
    }
private:
    std::map<int,StuffDone::SDFMap::iterator> pointers_;
    std::array<StuffDone::type,90> magicPointers_;
};

#endif

