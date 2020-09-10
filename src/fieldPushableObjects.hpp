#ifndef FIELD_PUSHABLE_OBJECTS_HPP
#define FIELD_PUSHABLE_OBJECTS_HPP

#include "scenario/item_variety.hpp"
#include "game/boe.locutils.hpp"
#include "fields.hpp"

namespace fields
{

template<eFieldType Type> struct PushableObjectStrings
{
    static void failToPush() {}
    static void pushed() {}
};

template<> struct PushableObjectStrings<OBJECT_CRATE>
{
    static void failToPush() {add_string_to_buf("  Can't push crate.");}
    static void pushed() {add_string_to_buf("  You push the crate.");}
};

template<> struct PushableObjectStrings<OBJECT_BARREL>
{
    static void failToPush() {add_string_to_buf("  Can't push barrel.");}
    static void pushed() {add_string_to_buf("  You push the barrel.");}
};

template<> struct PushableObjectStrings<OBJECT_BLOCK>
{
    static void failToPush() {add_string_to_buf("  Can't push block.");}
    static void pushed() {add_string_to_buf("  You push the block.");}
};

//TODO: Finish monster harmonization
template<eFieldType Type, bool IsPc, bool Moving, typename Source> bool executePush(location from_loc, location where_check, Source& source)
{
    if(source.template testField<Type>(where_check.x,where_check.y))
    {
        location to_loc = push_loc(from_loc,where_check);
        if(IsPc && !Moving && from_loc == to_loc)
        {
            PushableObjectStrings<Type>::failToPush();
            return false;
        }
        if(IsPc) PushableObjectStrings<Type>::pushed();
        source.template clearFields<Type>(where_check.x,where_check.y);
        if(to_loc.x > 0) source.template setField<Type>(to_loc.x,to_loc.y);
        if(!fieldgroups::ContainerFields::template test<Type>()) return true;
        source.template moveItems<true>(where_check,to_loc);
    }
    return true;
}

template<eFieldType... Objects> struct PushObjects
{
    template<bool IsPc, bool Moving, typename Source> static bool push(location from_loc, location where, Source& source)
    {
        return true;
    }
    template<typename Source> static void breakContainer(Source& source, location loc) {}
};

template<eFieldType Object, eFieldType... Objects> struct PushObjects<Object,Objects...>
{
    static constexpr uint64_t mask = util::BuildMask<Object,Objects...>::mask;
    template<bool IsPc, bool Moving, typename Source> static bool push(location from_loc, location where, Source& source)
    {
        if(!executePush<Object,IsPc,Moving>(from_loc,where,source)) return false;
        return PushObjects<Objects...>::template push<IsPc,Moving>(from_loc,where,source);
    }
    template<typename Source> static void breakContainer(Source& source, location loc)
    {
        if(fieldgroups::ContainerFields::template test<Object>() && source.template testField<Object>(loc.x,loc.y))
        {
            source.template clearFields<Object>(loc.x,loc.y);
            source.uncontainItems(loc);
        }
    }
};

typedef PushObjects<OBJECT_CRATE,OBJECT_BARREL,OBJECT_BLOCK> PushableObjects;

}//End namespace fields

#endif

