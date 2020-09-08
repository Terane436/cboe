#ifndef FIELD_GRAPHICS_HPP
#define FIELD_GRAPHICS_HPP

#include "fields.hpp"
#include "game/boe.text.hpp"

extern long anim_ticks;

struct BaseFieldGraphics
{
    static constexpr int GraphicRectX = 0;
    static constexpr int GraphicRectY = 0;
    static constexpr unsigned int GraphicAdjust = 0xFFFFFFFF;
    static constexpr int AnimCount = 1;
};

template<eFieldType Field> struct FieldGraphics : public BaseFieldGraphics {};

template<> struct FieldGraphics<WALL_FORCE> : public BaseFieldGraphics
{
    static constexpr int GraphicRectY = 1;
};

template<> struct FieldGraphics<WALL_FIRE> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 1;
    static constexpr int GraphicRectY = 1;
};

template<> struct FieldGraphics<FIELD_ANTIMAGIC> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 2;
    static constexpr int GraphicRectY = 1;
};

template<> struct FieldGraphics<CLOUD_STINK> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 3;
    static constexpr int GraphicRectY = 1;
};

template<> struct FieldGraphics<WALL_ICE> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 4;
    static constexpr int GraphicRectY = 1;
};

template<> struct FieldGraphics<WALL_BLADES> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 5;
    static constexpr int GraphicRectY = 1;
};

template<> struct FieldGraphics<CLOUD_SLEEP> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 6;
    static constexpr int GraphicRectY = 1;
};

template<> struct FieldGraphics<OBJECT_BLOCK> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 3;
    static constexpr int GraphicRectY = 0;
};

template<> struct FieldGraphics<SPECIAL_SPOT> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 4;
    static constexpr int GraphicRectY = 0;
};

template<> struct FieldGraphics<FIELD_WEB> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 5;
    static constexpr int GraphicRectY = 0;
};

template<> struct FieldGraphics<OBJECT_CRATE> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 6;
    static constexpr int GraphicRectY = 0;
};

template<> struct FieldGraphics<OBJECT_BARREL> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 7;
    static constexpr int GraphicRectY = 0;
};

template<> struct FieldGraphics<FIELD_QUICKFIRE> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 7;
    static constexpr int GraphicRectY = 1;
};

template<> struct FieldGraphics<BARRIER_FIRE> : public BaseFieldGraphics
{  //Animated fields
    static constexpr int GraphicRectX = 8;
    static constexpr int GraphicRectY = 4;
    static constexpr int AnimCount = 4;
};

template<> struct FieldGraphics<BARRIER_FORCE> : public BaseFieldGraphics
{  //Animated fields
    static constexpr int GraphicRectX = 8;
    static constexpr int GraphicRectY = 4;
    static constexpr int AnimCount = 4;
};

template<> struct FieldGraphics<SFX_SMALL_BLOOD> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 0;
    static constexpr int GraphicRectY = 3;
};

template<> struct FieldGraphics<SFX_MEDIUM_BLOOD> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 1;
    static constexpr int GraphicRectY = 3;
};

template<> struct FieldGraphics<SFX_LARGE_BLOOD> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 2;
    static constexpr int GraphicRectY = 3;
};

template<> struct FieldGraphics<SFX_SMALL_SLIME> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 3;
    static constexpr int GraphicRectY = 3;
};

template<> struct FieldGraphics<SFX_LARGE_SLIME> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 4;
    static constexpr int GraphicRectY = 3;
};

template<> struct FieldGraphics<SFX_ASH> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 5;
    static constexpr int GraphicRectY = 3;
};

template<> struct FieldGraphics<SFX_BONES> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 6;
    static constexpr int GraphicRectY = 3;
};

template<> struct FieldGraphics<SFX_RUBBLE> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 7;
    static constexpr int GraphicRectY = 3;
};

template<> struct FieldGraphics<BARRIER_CAGE> : public BaseFieldGraphics
{
    static constexpr int GraphicRectX = 1;
    static constexpr int GraphicRectY = 0;
};

template<eFieldType... Fields> struct DrawFields
{
    template<typename Source>
    static void draw(const Source& source, const sf::Texture& src_gworld, sf::RenderTarget& targ_gworld, location target, location where_draw, char masked, short main_win) {}

};

template<eFieldType Field, eFieldType... Fields> struct DrawFields<Field,Fields...>
{
    template<typename Source>
    static void draw(const Source& source, const sf::Texture& src_gworld, sf::RenderTarget& targ_gworld, location target, location where_draw, char masked, short main_win)
    {
        if(source.template testField<Field>(target.x,target.y))
        {
            int x = FieldGraphics<Field>::GraphicRectX;
            if(FieldGraphics<Field>::AnimCount > 1) x += (anim_ticks%FieldGraphics<Field>::AnimCount);
            Draw_Some_Item(src_gworld, calc_rect(x,FieldGraphics<Field>::GraphicRectY),
                targ_gworld,where_draw,masked,main_win,FieldGraphics<Field>::GraphicAdjust);
        }
    	DrawFields<Fields...>::draw(source,src_gworld,targ_gworld,target,where_draw,masked,main_win);
    }
};

typedef DrawFields<WALL_FORCE,WALL_FIRE,FIELD_ANTIMAGIC,CLOUD_STINK,WALL_ICE,WALL_BLADES,CLOUD_SLEEP,OBJECT_BLOCK,SPECIAL_SPOT,FIELD_WEB,OBJECT_CRATE,OBJECT_BARREL,FIELD_QUICKFIRE,SFX_SMALL_BLOOD,SFX_MEDIUM_BLOOD,SFX_LARGE_BLOOD,SFX_SMALL_SLIME,SFX_LARGE_SLIME,SFX_ASH,SFX_BONES,SFX_RUBBLE,BARRIER_CAGE> StandardFieldDrawing;

typedef DrawFields<BARRIER_FORCE,BARRIER_FIRE> AnimatedFieldDrawing;

#endif

