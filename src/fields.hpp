//
//  fields.hpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-13.
//
//

#ifndef BoE_FIELDS_HPP
#define BoE_FIELDS_HPP

#include "utility/templateutil.hpp"
#include "damage.hpp"
#include "mathutil.hpp"

// This is a slight misnomer, as a couple of these are not true fields.
enum eFieldType {
	SPECIAL_EXPLORED = 0,
	WALL_FORCE = 1,
	WALL_FIRE = 2,
	FIELD_ANTIMAGIC = 3,
	CLOUD_STINK = 4,
	WALL_ICE = 5,
	WALL_BLADES = 6,
	CLOUD_SLEEP = 7,
	// Begin fields saved in town setup
	OBJECT_BLOCK = 8,
	SPECIAL_SPOT = 9, // Space contains a white special spot
	FIELD_WEB = 10,
	OBJECT_CRATE = 11,
	OBJECT_BARREL = 12,
	BARRIER_FIRE = 13,
	BARRIER_FORCE = 14,
	FIELD_QUICKFIRE = 15,
	// End fields saved in town setup
	SFX_SMALL_BLOOD = 16,
	SFX_MEDIUM_BLOOD = 17,
	SFX_LARGE_BLOOD = 18,
	SFX_SMALL_SLIME = 19,
	SFX_LARGE_SLIME = 20,
	SFX_ASH = 21,
	SFX_BONES = 22,
	SFX_RUBBLE = 23,
	BARRIER_CAGE = 24,
	SPECIAL_ROAD = 25,
	// From here on are special values that don't index anything.
	// Thus, they start at 32.
	FIELD_DISPEL = 32, // Dispel field
	FIELD_SMASH = 33, // Move Mountains
	// Mustn't have anything >= 50
};

// Field types are used to index bit fields - use util::BuildMask at all times with them

typedef unsigned long FieldBitmap;

std::ostream& operator << (std::ostream& out, eFieldType e);
std::istream& operator >> (std::istream& in, eFieldType& e);

//TEMPLATE SUPPORT FOR CONSISTENT APPLICATION OF FIELD EFFECTS
template<typename T> struct FieldDamager
{
    static void damage(T&, short, eDamageType, short = 7) {}
};

struct BaseFieldsControl //BARRIER_CAGE, SPECIAL_ROAD, SPECIAL_SPOT
{
    typedef util::BuildMask<> BlockFields;
    typedef util::BuildMask<> ClearFields;
    static constexpr bool Checked = true;
    static constexpr bool AvoidImpassable = false;
    static constexpr short AntimagicChance = 0;
    static constexpr bool IsSfx = false;
    static constexpr bool TriggeringClears = false;
    static constexpr short FadeChance = 0;
    template<typename Target, bool Init> static void applyFieldEffect(Target&) {}
};

template<eFieldType Field> struct FieldControls : public BaseFieldsControl {};

struct FieldControlsObject : public BaseFieldsControl
{
    typedef util::BuildMask<BARRIER_FIRE,BARRIER_FORCE,FIELD_QUICKFIRE> BlockFields;
};

struct FieldControlsSfx : public BaseFieldsControl
{
    static constexpr bool IsSfx = true;
    typedef util::BuildMask<SFX_SMALL_BLOOD,SFX_MEDIUM_BLOOD,SFX_LARGE_BLOOD,SFX_SMALL_SLIME,SFX_LARGE_SLIME,SFX_ASH,SFX_BONES,SFX_RUBBLE> ClearFields;
};

template<> struct FieldControls<WALL_FORCE> : public BaseFieldsControl
{
    typedef util::BuildMask<FIELD_ANTIMAGIC,WALL_BLADES,FIELD_QUICKFIRE,BARRIER_FIRE,BARRIER_FORCE> BlockFields;//CHANGE: Removed OBJECT_CRATE, OBJECT_BARREL from blocking
    typedef util::BuildMask<FIELD_WEB> ClearFields;//CHANGE: Removed WALL_FIRE from cleared fields
    static constexpr bool AvoidImpassable = true;
    static constexpr short FadeChance = 6;
    template<typename Target, bool Init>
    static void applyFieldEffect(Target& target) {FieldDamager<Target>::damage(target,get_ran(3-(Init?1:0),1,6),eDamageType::MAGIC);}
};

template<> struct FieldControls<WALL_FIRE> : public BaseFieldsControl
{ //CHANGE: Removed OBJECT_CRATE, OBJECT_BARREL, FIELD_WEB, CLOUD_SLEEP, CLOUD_STINK, WALL_FORCE from blocking
    typedef util::BuildMask<FIELD_ANTIMAGIC,WALL_BLADES,FIELD_QUICKFIRE,WALL_ICE,BARRIER_FIRE,BARRIER_FORCE> BlockFields;
    typedef util::BuildMask<FIELD_WEB> ClearFields;
    static constexpr bool AvoidImpassable = true;
    static constexpr short FadeChance = 4;
    template<typename Target,bool Init> static void applyFieldEffect(Target& target) {FieldDamager<Target>::damage(target,get_ran(2-(Init ?1:0),1,6)+1,eDamageType::FIRE);}
};

template<> struct FieldControls<FIELD_ANTIMAGIC> : public BaseFieldsControl
{ //CHANGE: Removed CLOUD_STINK, CLOUD_SLEEP from cleared fields
    typedef util::BuildMask<FIELD_ANTIMAGIC,FIELD_QUICKFIRE,BARRIER_FIRE,BARRIER_FORCE> BlockFields;
    typedef util::BuildMask<WALL_FIRE,WALL_ICE,WALL_BLADES,WALL_FORCE> ClearFields;
    static constexpr bool AvoidImpassable = true;
    static constexpr short FadeChance = 8;
};

template<> struct FieldControls<CLOUD_STINK> : public BaseFieldsControl
{ //CHANGE: Removed all walls, CLOUD_SLEEP from blocking
    typedef util::BuildMask<FIELD_ANTIMAGIC,FIELD_QUICKFIRE,BARRIER_FIRE,BARRIER_FORCE> BlockFields;
    static constexpr bool AvoidImpassable = true;
    static constexpr short FadeChance = 4;
    template<typename Target, bool Init> static void applyFieldEffect(Target& target) {target.curse(get_ran(1,1,2));}
};

template<> struct FieldControls<WALL_ICE> : public BaseFieldsControl
{ //CHANGE: Removed WALL_FORCE, WALL_BLADES, FIELD_WEB, OBJECT_CRATE, OBJECT_BARREL from blocking, added CLOUD_SLEEP to clear
    typedef util::BuildMask<FIELD_ANTIMAGIC,FIELD_QUICKFIRE,BARRIER_FIRE,BARRIER_FORCE> BlockFields;
    typedef util::BuildMask<WALL_FIRE,CLOUD_STINK,CLOUD_SLEEP> ClearFields;
    static constexpr bool AvoidImpassable = true;
    static constexpr short FadeChance = 6;
    template<typename Target, bool Init> static void applyFieldEffect(Target& target) {FieldDamager<Target>::damage(target,get_ran(3-(Init ?1:0),1,6),eDamageType::COLD);}
};

template<> struct FieldControls<WALL_BLADES> : public BaseFieldsControl
{ //CHANGE: Removed WALL_FORCE, WALL_FIRE from cleared fields, added FIELD_WEB
    typedef util::BuildMask<FIELD_ANTIMAGIC,FIELD_QUICKFIRE,BARRIER_FIRE,BARRIER_FORCE> BlockFields;
    typedef util::BuildMask<FIELD_WEB> ClearFields;
    static constexpr bool AvoidImpassable = true;
    static constexpr short FadeChance = 5;
    template<typename Target,bool Init> static void applyFieldEffect(Target& target) {FieldDamager<Target>::damage(target,get_ran(6-(Init ?2:0),1,8),eDamageType::WEAPON);}
};

template<> struct FieldControls<CLOUD_SLEEP> : public BaseFieldsControl
{ //CHANGE: Removed WALL_FORCE, WALL_FIRE from cleared fields
    typedef util::BuildMask<FIELD_ANTIMAGIC,FIELD_QUICKFIRE,BARRIER_FIRE,BARRIER_FORCE> BlockFields;
    static constexpr bool AvoidImpassable = true;
    static constexpr short FadeChance = 4;
    template<typename Target, bool Init> static void applyFieldEffect(Target& target) {target.sleep(eStatus::ASLEEP,3,0);}
};

template<> struct FieldControls<FIELD_WEB> : public BaseFieldsControl
{ //CHANGE: Removed CLOUD_SLEEP from blocking
    typedef util::BuildMask<FIELD_ANTIMAGIC,WALL_BLADES,WALL_FIRE,WALL_ICE,FIELD_QUICKFIRE,BARRIER_FIRE,BARRIER_FORCE> BlockFields;
    static constexpr bool AvoidImpassable = true;
    template<typename Target, bool Init> static void applyFieldEffect(Target& target) {target.web(get_ran(1,2,3));}//CHANGE: Slight shift in web levels
    static constexpr bool TriggeringClears = true;
};

template<> struct FieldControls<OBJECT_BLOCK> : public FieldControlsObject
{
    template<typename Target, bool Init> static void applyFieldEffect(Target& target) {FieldDamager<Target>::damage(target,get_ran(6,1,8),eDamageType::WEAPON);}
};
template<> struct FieldControls<OBJECT_CRATE> : public FieldControlsObject {};
template<> struct FieldControls<OBJECT_BARREL> : public FieldControlsObject {};

template<> struct FieldControls<BARRIER_FIRE> : public BaseFieldsControl
{
    typedef util::BuildMask<FIELD_QUICKFIRE,OBJECT_CRATE,OBJECT_BARREL,BARRIER_FORCE> BlockFields;
    typedef util::BuildMask<WALL_FIRE,FIELD_WEB,WALL_FORCE,FIELD_ANTIMAGIC,CLOUD_STINK,WALL_ICE,WALL_BLADES,CLOUD_SLEEP> ClearFields;
    static constexpr bool Checked = true;
    static constexpr bool AvoidImpassable = true;
    static constexpr short AntimagicChance = 3;
};

template<> struct FieldControls<BARRIER_FORCE> : public BaseFieldsControl
{
    typedef util::BuildMask<BARRIER_FIRE,FIELD_QUICKFIRE,OBJECT_CRATE,OBJECT_BARREL> BlockFields;
    typedef util::BuildMask<WALL_FIRE,FIELD_WEB,WALL_FORCE,FIELD_ANTIMAGIC,CLOUD_STINK,WALL_ICE,WALL_BLADES,CLOUD_SLEEP> ClearFields;
    static constexpr bool Checked = true;
    static constexpr bool AvoidImpassable = true;
    static constexpr short AntimagicChance = 2;
};

template<> struct FieldControls<FIELD_QUICKFIRE> : public BaseFieldsControl
{ //CHANGE: Terrain blockages changed slightly
    typedef util::BuildMask<BARRIER_FORCE,BARRIER_FIRE> BlockFields;
    typedef util::BuildMask<WALL_FIRE,FIELD_WEB,WALL_FORCE,FIELD_ANTIMAGIC,CLOUD_STINK,WALL_ICE,WALL_BLADES,CLOUD_SLEEP,OBJECT_CRATE,OBJECT_BARREL,BARRIER_FORCE,BARRIER_FIRE> ClearFields;
    static constexpr bool Checked = true;
    static constexpr bool AvoidImpassable = true;
    static constexpr short AntimagicChance = 1;
    template<typename Target, bool Init> static void applyFieldEffect(Target& target) {FieldDamager<Target>::damage(target,get_ran(2,1,8),eDamageType::FIRE);}
};

template<> struct FieldControls<SFX_SMALL_BLOOD> : public FieldControlsSfx
{
    typedef util::BuildMask<SFX_MEDIUM_BLOOD,SFX_LARGE_BLOOD> BlockFields;
};

template<> struct FieldControls<SFX_MEDIUM_BLOOD> : public FieldControlsSfx
{
    typedef util::BuildMask<SFX_LARGE_BLOOD> BlockFields;
};

template<> struct FieldControls<SFX_LARGE_BLOOD> : public FieldControlsSfx {};

template<> struct FieldControls<SFX_SMALL_SLIME> : public FieldControlsSfx
{
    typedef util::BuildMask<SFX_LARGE_SLIME> BlockFields;
};

template<> struct FieldControls<SFX_LARGE_SLIME> : public FieldControlsSfx {};

template<> struct FieldControls<SFX_ASH> : public FieldControlsSfx {};
template<> struct FieldControls<SFX_BONES> : public FieldControlsSfx {};
template<> struct FieldControls<SFX_RUBBLE> : public FieldControlsSfx {};

namespace fieldgroups {
typedef util::BuildMask<SPECIAL_SPOT,OBJECT_CRATE,OBJECT_BARREL,OBJECT_BLOCK,FIELD_QUICKFIRE,WALL_FORCE,WALL_FIRE,FIELD_ANTIMAGIC,CLOUD_STINK,WALL_ICE,WALL_BLADES,CLOUD_SLEEP> NonClearFields;
typedef util::BuildMask<WALL_FORCE,WALL_FIRE,FIELD_ANTIMAGIC,CLOUD_STINK,CLOUD_SLEEP,WALL_ICE,WALL_BLADES> FadingFields;
}

//MORE TEMPLATE SUPPORT FOR CONSISTENT FIELD EFFECT APPLICATION
template<eFieldType... Fields> struct FieldApplication
{
    template<typename Type, typename TestSource> static void inflict(Type& target, short x, short y, TestSource& source, eFieldType ignoreField = SPECIAL_EXPLORED) {}
};

template<eFieldType Field> struct FieldApplication<Field>
{
    template<typename Type, typename TestSource> static void inflict(Type& target, short x, short y, TestSource& source, eFieldType ignoreField = SPECIAL_EXPLORED)
    {
        if(Field != ignoreField && source.template testField<Field>(x,y))
	{
            FieldControls<Field>::template applyFieldEffect<Type,false>(target);
	    if(FieldControls<Field>::TriggeringClears) source.template clearFields<Field>(x,y);
	}
    }
};

template<eFieldType Field, eFieldType... Fields> struct FieldApplication<Field,Fields...>
{
    template<typename Type, typename TestSource> static void inflict(Type& target, short x, short y, TestSource& source, eFieldType ignoreField = SPECIAL_EXPLORED)
    {
        if(Field != ignoreField && source.template testField<Field>(x,y))
	{
            FieldControls<Field>::template applyFieldEffect<Type,false>(target);
	    if(FieldControls<Field>::TriggeringClears) source.template clearFields<Field>(x,y);
	}
	FieldApplication<Fields...>::inflict(target,x,y,source,ignoreField);
    }
};

typedef FieldApplication<WALL_FORCE,WALL_FIRE,CLOUD_STINK,CLOUD_SLEEP,WALL_ICE,WALL_BLADES,FIELD_QUICKFIRE,FIELD_WEB> FieldApplier;

#endif
