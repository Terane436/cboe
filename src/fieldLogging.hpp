#ifndef FIELD_LOGGING_HPP
#define FIELD_LOGGING_HPP

#include <iostream>
#include "fields.hpp"
#include <sstream>

namespace fields
{

template<bool FieldNaming> void nameField(short type, std::ostringstream& sout)
{
    switch(type)
    {
    case eFieldType::SPECIAL_EXPLORED:
    case eFieldType::SPECIAL_SPOT:
    case eFieldType::SPECIAL_ROAD:
        break; // These are invalid field types
    case eFieldType::CLOUD_SLEEP: sout << (FieldNaming ? "Sleep" : "sleep fields"); break;
    case eFieldType::CLOUD_STINK: sout << (FieldNaming ? "Foul" : "stinking clouds"); break;
    case eFieldType::WALL_FIRE: sout << (FieldNaming ? "Fiery" : "fire fields"); break;
    case eFieldType::WALL_FORCE: sout << (FieldNaming ? "Charged" : "shock fields"); break;
    case eFieldType::WALL_ICE: sout << (FieldNaming ? "Frosted" : "ice fields"); break;
    case eFieldType::WALL_BLADES: sout << (FieldNaming ? "Thorny" : "blade fields"); break;
    case eFieldType::FIELD_ANTIMAGIC: sout << (FieldNaming ? "Null" : "antimagic fields"); break;
    case eFieldType::FIELD_WEB: sout << (FieldNaming ? "Web" : "webs"); break;
    case eFieldType::FIELD_QUICKFIRE: sout << (FieldNaming ? "Incendiary" : "quickfire"); break;
    case eFieldType::BARRIER_CAGE: sout << (FieldNaming ? "Entrapping" : "forcecages"); break;
    case eFieldType::BARRIER_FIRE: case eFieldType::BARRIER_FORCE: sout << (FieldNaming ? "Barrier" : "barriers"); break;
    case eFieldType::FIELD_DISPEL:
        if(FieldNaming) sout << "Dispelling";
        else sout.str("Dispels surrounding fields");
        break;
    case eFieldType::FIELD_SMASH:
        if(FieldNaming) sout << "Smashing";
        else sout.str("Wall-smashing aura");
        break;
    case eFieldType::OBJECT_BARREL: sout << (FieldNaming ?  "Barrel" : "barrels"); break;
    case eFieldType::OBJECT_BLOCK: sout << (FieldNaming ? "Stone Block" : "stone blocks"); break;
    case eFieldType::OBJECT_CRATE: sout << (FieldNaming ? "Crate" : "crates"); break;
    case eFieldType::SFX_ASH: case eFieldType::SFX_BONES: case eFieldType::SFX_RUBBLE:
    case eFieldType::SFX_SMALL_BLOOD: case eFieldType::SFX_MEDIUM_BLOOD: case eFieldType::SFX_LARGE_BLOOD:
    case eFieldType::SFX_SMALL_SLIME: case eFieldType::SFX_LARGE_SLIME:
        sout << (FieldNaming ? "Littering" : "litter");
        break;
    }
}

}//End namespace fields

#endif

