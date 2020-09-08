#ifndef FIELD_INFLICTS_HPP
#define FIELD_INFLICTS_HPP

#include "fields.hpp"
#include <type_traits>

//When adding a new field type, if it is intended to have effects, define a specialization here
template<eFieldType Field> struct FieldEffects {template<typename Target, bool Init> static void applyFieldEffect(Target&) {}};
template<> struct FieldEffects<WALL_FORCE> {
    template<typename Target,bool Init> static void applyFieldEffect(Target& target) {FieldDamager<Target>::damage(target,get_ran(3-(Init?1:0),1,6),eDamageType::MAGIC);}
};
template<> struct FieldEffects<WALL_FIRE> {
    template<typename Target,bool Init> static void applyFieldEffect(Target& target) {FieldDamager<Target>::damage(target,get_ran(2-(Init ?1:0),1,6)+1,eDamageType::FIRE);}
};
template<> struct FieldEffects<CLOUD_STINK> {
    template<typename Target, bool Init> static void applyFieldEffect(Target& target) {target.curse(get_ran(1,1,2));}
};
template<> struct FieldEffects<WALL_ICE> {
    template<typename Target, bool Init> static void applyFieldEffect(Target& target) {FieldDamager<Target>::damage(target,get_ran(3-(Init ?1:0),1,6),eDamageType::COLD);}
};
template<> struct FieldEffects<WALL_BLADES> {
    template<typename Target,bool Init> static void applyFieldEffect(Target& target) {FieldDamager<Target>::damage(target,get_ran(6-(Init ?2:0),1,8),eDamageType::WEAPON);}
};
template<> struct FieldEffects<CLOUD_SLEEP> {
    template<typename Target, bool Init> static void applyFieldEffect(Target& target) {target.sleep(eStatus::ASLEEP,3,0);}
};
template<> struct FieldEffects<FIELD_WEB> {
    template<typename Target, bool Init> static void applyFieldEffect(Target& target) {target.web(get_ran(1,2,3));}//CHANGE: Slight shift in web levels
};
template<> struct FieldEffects<OBJECT_BLOCK> {
    template<typename Target, bool Init> static void applyFieldEffect(Target& target) {FieldDamager<Target>::damage(target,get_ran(6,1,8),eDamageType::WEAPON);}
};//Unusual case, applied only initially
template<> struct FieldEffects<BARRIER_FIRE> {
    template<typename Target, bool Init> static void applyFieldEffect(Target& target) {FieldDamager<Target>::damage(target,get_ran(2,1,10),eDamageType::MAGIC);}
};
template<> struct FieldEffects<FIELD_QUICKFIRE> {
    template<typename Target, bool Init> static void applyFieldEffect(Target& target) {FieldDamager<Target>::damage(target,get_ran(2,1,8),eDamageType::FIRE);}
};

//MORE TEMPLATE SUPPORT FOR CONSISTENT FIELD EFFECT APPLICATION
template<eFieldType... Fields> struct FieldApplication
{
    template<typename Type, typename TestSource, bool Log = false, bool Reduce = false> static void inflict(Type& target, short x, short y, TestSource& source, eFieldType ignoreField = SPECIAL_EXPLORED) {}
};

template<eFieldType Field> struct FieldApplication<Field>
{
    template<typename Type, typename TestSource, bool Log = false, bool Reduce = false, bool NoClear = false>
    static void inflict(Type& target, short x, short y, TestSource& source, eFieldType ignoreField = SPECIAL_EXPLORED)
    {
        if(Field != ignoreField && source.template testField<Field>(x,y))
	{
            if(Log) add_string_to_buf(FieldControls<Field>::log());
            FieldEffects<Field>::template applyFieldEffect<Type,Reduce>(target);
	    if(!NoClear && FieldControls<Field>::TriggeringClears) source.template clearFields<Field>(x,y);
	}
    }
    template<typename TestSource> static void clearTriggeredFields(TestSource& source, short x, short y)
    {if(FieldControls<Field>::TriggeringClears) source.template clearFields<Field>(x,y);}
    template<typename TestSource> static void buildLog(const TestSource& source, short x, short y)
    {
        if(source.template testField<Field>(x,y)) add_string_to_buf(FieldControls<Field>::log());
    }
    template<typename TestSource> static void buildLook(const TestSource& source, short x, short y)
    {
        if(source.template testField<Field>(x,y)) add_string_to_buf(FieldControls<Field>::look());
    }
};

template<eFieldType Field, eFieldType... Fields> struct FieldApplication<Field,Fields...>
{
    static constexpr uint64_t mask = util::BuildMask<Field,Fields...>::mask;
    template<typename Type, typename TestSource, bool Log = false, bool Reduce = false, bool NoClear = false>
    static void inflict(Type& target, short x, short y, TestSource& source, eFieldType ignoreField = SPECIAL_EXPLORED)
    {
        if(Field != ignoreField && source.template testField<Field>(x,y))
	{
            if(Log) add_string_to_buf(FieldControls<Field>::log());
            FieldEffects<Field>::template applyFieldEffect<Type,Reduce>(target);
	    if(!NoClear && FieldControls<Field>::TriggeringClears) source.template clearFields<Field>(x,y);
	}
	FieldApplication<Fields...>::template inflict<Type,TestSource,Log,Reduce,NoClear>(target,x,y,source,ignoreField);
    }
    template<typename TestSource> static void clearTriggeredFields(TestSource& source, short x, short y)
    {
        if(FieldControls<Field>::TriggeringClears) source.template clearFields<Field>(x,y);
        FieldApplication<Fields...>::clearTriggeredFields(source,x,y);
    }
    template<typename TestSource> static void buildLog(const TestSource& source, short x, short y)
    {
        if(source.template testField<Field>(x,y)) add_string_to_buf(FieldControls<Field>::log());
        FieldApplication<Fields...>::buildLog(source,x,y);
    }
    template<typename TestSource> static void buildLook(const TestSource& source, short x, short y)
    {
        if(source.template testField<Field>(x,y)) add_string_to_buf(FieldControls<Field>::look());
        FieldApplication<Fields...>::buildLook(source,x,y);
    }
};

//All field types with effects should be listed here
typedef FieldApplication<WALL_FORCE,WALL_FIRE,CLOUD_STINK,CLOUD_SLEEP,WALL_ICE,WALL_BLADES,FIELD_QUICKFIRE,FIELD_WEB,BARRIER_FIRE> FieldApplier;

template<typename Target, bool Init = std::is_same<Target,cPlayer>::value> void executeEffects(int effect, Target& target, short source)
{ //When adding a new field with effects, add it here as well
    if(effect == 0) return;
    switch(effect)
    { 
    case WALL_FORCE:
        FieldEffects<WALL_FORCE>::template applyFieldEffect<Target,Init>(target);
        break;
    case WALL_FIRE:
        FieldEffects<WALL_FIRE>::template applyFieldEffect<Target,Init>(target);
        break;
    case WALL_ICE:
        FieldEffects<WALL_ICE>::template applyFieldEffect<Target,Init>(target);
        break;
    case WALL_BLADES:
        FieldEffects<WALL_BLADES>::template applyFieldEffect<Target,Init>(target);
        break;
    case OBJECT_BLOCK:
        FieldEffects<OBJECT_BLOCK>::template applyFieldEffect<Target,Init>(target);
        break;
    case FIELD_WEB:
        FieldEffects<FIELD_WEB>::template applyFieldEffect<Target,Init>(target);
        break;
    case CLOUD_STINK:
        FieldEffects<CLOUD_STINK>::template applyFieldEffect<Target,Init>(target);
        break;
    case CLOUD_SLEEP:
        FieldEffects<CLOUD_SLEEP>::template applyFieldEffect<Target,Init>(target);
        break;
    case BARRIER_CAGE:
        if(std::is_same<Target,cCreature>::value) //Special case, only monsters get force-caged here for some reason.  TODO: Consider consistency
            target.status[eStatus::FORCECAGE] = max(8, target.status[eStatus::FORCECAGE]);
        break;
    default:
	if(effect < 256) break;
        eDamageType type = (eDamageType)((effect >> 8)-1);
        unsigned short dice = effect & 0x0FF;
        if(type == eDamageType::MARKED) break;
        FieldDamager<Target>::damage(target,get_ran(dice,1,6),type,source);
        break;
    }
}

typedef FieldApplication<FIELD_WEB,OBJECT_CRATE,OBJECT_BARREL,OBJECT_BLOCK,BARRIER_FIRE,BARRIER_FORCE,FIELD_QUICKFIRE,WALL_FIRE,WALL_FORCE,FIELD_ANTIMAGIC,CLOUD_STINK,CLOUD_SLEEP,WALL_ICE,WALL_BLADES,BARRIER_CAGE,
	SFX_SMALL_BLOOD,SFX_MEDIUM_BLOOD,SFX_LARGE_BLOOD,SFX_SMALL_SLIME,SFX_LARGE_SLIME,SFX_ASH,SFX_BONES,SFX_RUBBLE>
    VisibleFieldApplication;

#endif

