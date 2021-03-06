/*
 *  universe.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#ifndef BOE_DATA_UNIVERSE_H
#define BOE_DATA_UNIVERSE_H

#include <iosfwd>
#include <memory>
#include <set>
#include <array>
#include <boost/filesystem/path.hpp>
#include "party.hpp"
#include "population.hpp"
#include "item.hpp"
#include "town.hpp"
#include "talking.hpp"
#include "scenario.hpp"
#include "pictypes.hpp"
#include "fields.hpp"
#include "utility/templateutil.hpp"
#include "mathutil.hpp"

namespace legacy {
	struct out_info_type;
	struct current_town_type;
	struct town_item_list;
	struct stored_town_maps_type;
	struct stored_outdoor_maps_type;
	struct big_tr_type;
};

class cCurTown {
	short cur_talk_loaded = -1;
	bool free_for_sfx(short x, short y);
	cUniverse& univ;
	cTown* arena;
	cTown*const record() const;
public:
	fields::FieldBitmap savedFields(short x, short y) {return (fields[x][y]>>8) & 0x0FF;}
	void zeroField(short x, short y) {fields[x][y] = 0;}
        void directSetFields(short x, short y, fields::FieldBitmap mask) {fields[x][y] |= mask;}
	bool testField(short x, short y, int field) const
	{
            if(x > record()->max_dim || y > record()->max_dim) return false;
            return fields[x][y] & (0x01ULL << field);
	}
        template<typename Fields> bool testFields(short x, short y) const
        {
            if(x > record()->max_dim || y > record()->max_dim) return false;
            return fields[x][y] & Fields::mask;
        }
        template<fields::eFieldType... Fields> bool testField(short x, short y) const
        {
            if(x > record()->max_dim || y > record()->max_dim) return false;
            return fields[x][y] & util::BuildMask<Fields...>::mask;
        }
        template<fields::eFieldType... Fields> bool testFieldUnchecked(short x, short y) const
        {
            return fields[x][y] & util::BuildMask<Fields...>::mask;
        }
	template<fields::eFieldType... Fields> bool testFieldAll(short x, short y) const
        {
            if(x > record()->max_dim || y > record()->max_dim) return false;
            return (fields[x][y] & util::BuildMask<Fields...>::mask) == util::BuildMask<Fields...>::mask;
        }
	template<typename SetField, typename BlockFields, typename ClearFields, bool Checked = false, bool AvoidImpassable = true, short AntimagicChance = 0, bool IsSfx = false>
	bool setFields(short x, short y)
	{ //Should never be used directly for any field that has Antimagic Chance
	  //Use this directly to set field combinations if desirable - to set a single field type always use setField below
            if(Checked && (x > record()->max_dim || y > record()->max_dim)) return false;
            if(AvoidImpassable && is_impassable(x,y)) return false;
            if(fields[x][y] & BlockFields::mask) return false;
	    if(AntimagicChance && get_ran(1,0,AntimagicChance) < AntimagicChance) return false;
	    if(IsSfx && !free_for_sfx(x,y)) return false;
	    fields[x][y] &= ~ClearFields::mask;	
	    fields[x][y] |= SetField::mask;
	    return true;
	}
	template<fields::eFieldType... Fields> void clearFields(short x, short y)
        {
            fields[x][y] &= ~util::BuildMask<Fields...>::mask;
        }
	template<typename FieldSet> void clearFields(short x, short y)
	{
            fields[x][y] &= ~FieldSet::mask;
	}
	template<fields::eFieldType Field, bool Set = true> bool setField(short x, short y)
	{
	    if(Set)
            { //If really important, consider special extra check for QUICKFIRE here
                bool r = setFields<util::BuildMask<Field>,
		     typename fields::FieldControls<Field>::BlockFields,
		     typename fields::FieldControls<Field>::ClearFields,
		     fields::FieldControls<Field>::Checked,
		     fields::FieldControls<Field>::AvoidImpassable,
		     fields::FieldControls<Field>::AntimagicChance,
		     fields::FieldControls<Field>::IsSfx>(x,y);
		if(Field == fields::FIELD_QUICKFIRE && r) quickfire_present = true;
		return r;
            }
	    else
	    {
                if(x > record()->max_dim || y > record()->max_dim) return false;
                clearFields<Field>(x,y);
		return true;
	    }
	}
        template<bool RequireContained>
	void moveItems(location from, location to)
	{
            for(auto& item : items)
            {
                if(item.variety != eItemType::NO_ITEM && item.item_loc == from)
                {
                    if(!RequireContained || (item.contained && item.held))
                        item.item_loc = to;
                }
            }
	}
        void uncontainItems(location loc)
        {
            for(auto& item : items)
            {
                if(item.variety != eItemType::NO_ITEM && item.item_loc == loc && item.contained)
                    item.contained = item.held = false;
            }
        }
        const std::vector<cItem>& getItems() const {return items;}

	bool quickfire_present = false, belt_present = false;
	// formerly current_town_type
	short difficulty;
	cPopulation monst;
	
	std::vector<cItem> items; // formerly town_item_list type
private:
	fields::FieldBitmap fields[64][64];
public:
	void import_legacy(legacy::current_town_type& old);
	void import_legacy(legacy::town_item_list& old);
	void import_legacy(unsigned char(& old_sfx)[64][64], unsigned char(& old_misc_i)[64][64]);
	void import_legacy(legacy::big_tr_type& old);
	
	cTown* operator->() {return record();}
	cTown& operator*() {return *record();}
	explicit cCurTown(cUniverse& univ);
	short countMonsters();
	cSpeech& cur_talk(); // Get the currently loaded speech
	bool prep_talk(short which); // Prepare for loading specified speech, returning true if already loaded
	void prep_arena(); // Set up for a combat arena
	void place_preset_fields();
	bool is_special(short x, short y) const;
	bool is_impassable(short x, short y);
	void writeTo(std::ostream& file) const;
	void readFrom(std::istream& file);
	
	~cCurTown();
	// It's not directly copyable due to the cUniverse reference, which must always point to the cUniverse that contains it.
	// The cUniverse copy constructor is thus responsible for performing the copy.
	cCurTown(const cCurTown&) = delete;
	cCurTown& operator=(const cCurTown&) = delete;
	// Not movable for similar reasons
	cCurTown(const cCurTown&& other) = delete;
	cCurTown& operator=(const cCurTown&& other) = delete;
	// This implements the actual copy/move.
	void copy(const cCurTown& other);
	void swap(cCurTown& other);
};

class cCurOut {
	cUniverse& univ;
public:
	char expl[96][96]; // formerly out_info_type
	ter_num_t out[96][96];
	unsigned char out_e[96][96];
	
	// These take global coords (ie 0..95)
	bool is_spot(int x, int y);
	bool is_road(int x, int y);
	
	void import_legacy(legacy::out_info_type& old);
	
	typedef ter_num_t arr_96[96];
	arr_96& operator [] (size_t i);
	ter_num_t& operator [] (location loc);
	void writeTo(std::ostream& file) const;
	void readFrom(std::istream& file);
	cOutdoors* operator->();
	explicit cCurOut(cUniverse& univ);
	// It's not directly copyable due to the cUniverse reference, which must always point to the cUniverse that contains it.
	// The cUniverse copy constructor is thus responsible for performing the copy.
	cCurOut(const cCurOut&) = delete;
	cCurOut& operator=(const cCurOut&) = delete;
	// Not movable for similar reasons
	cCurOut(const cCurOut&& other) = delete;
	cCurOut& operator=(const cCurOut&& other) = delete;
	// This implements the actual copy/move.
	void copy(const cCurOut& other);
	void swap(cCurOut& other);
};

enum eTargetType {TARG_ANY, TARG_PC, TARG_MONST};

class cUniverse{
	template<typename T> using update_info = std::set<T*>;
	// All these maps are transient data that doesn't need to be saved
	std::map<pic_num_t, update_info<cItem>> update_items;
	std::map<pic_num_t, update_info<cMonster>> update_monsters;
	std::map<pic_num_t, update_info<cPlayer>> update_pcs;
	std::map<pic_num_t, update_info<miss_num_t>> update_missiles;
	std::set<pic_num_t> used_graphics;
	pic_num_t addGraphic(pic_num_t pic, ePicType type);
	void check_monst(cMonster& monst);
	void check_item(cItem& item);
	// The string buffer currently isn't saved
	std::string strbuf;
	std::map<int,std::string> extrabufs;
	cItem get_random_store_item(int loot_type, bool allow_junk_treasure);
public:
	void exportSummons();
	void exportGraphics();
	
	iLiving& get_target(size_t which);
	iLiving* target_there(location pos, eTargetType type = TARG_ANY);
	size_t get_target_i(iLiving& who);
	
	std::string& get_buf() {return strbuf;}
	void swap_buf(int newbuf) {std::swap(strbuf, extrabufs[newbuf]);}
	
	unsigned char& cpn_flag(unsigned int x, unsigned int y, std::string id = "");
	
	short cur_pc = 0;
	cPlayer& current_pc();
	
	cScenario scenario;
	cParty party;
	std::map<long,cPlayer*> stored_pcs;
	cCurTown town;
	cCurOut out;
	fs::path file;
	bool debug_mode { false };
	bool ghost_mode { false };
	bool node_step_through { false };
	
	void clear_stored_pcs();
	void import_legacy(legacy::stored_town_maps_type& old);
	void import_legacy(legacy::stored_outdoor_maps_type& old);
	void enter_scenario(const std::string& name);
	void refresh_store_items();
	void generate_job_bank(int which, job_bank_t& bank);
	short difficulty_adjust() const;
	explicit cUniverse(ePartyPreset party_type = PARTY_DEFAULT);
	~cUniverse();
	// Copy-and-swap
	void swap(cUniverse& other);
	cUniverse(const cUniverse& other);
	cUniverse(cUniverse&& other);
	cUniverse& operator=(cUniverse other);
	static void(* print_result)(std::string);
};

#endif
