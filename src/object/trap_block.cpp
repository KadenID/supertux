//  SuperTux
//  Copyright (C) 2024
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.

#include "object/trap_block.hpp"

#include "audio/sound_manager.hpp"
#include "badguy/badguy.hpp"
#include "math/random.hpp"
#include "object/particles.hpp"
#include "object/player.hpp"
#include "util/reader_mapping.hpp"
#include "supertux/game_object_factory.hpp"
#include "supertux/sector.hpp"

TrapBlock::TrapBlock(const Vector& pos) :
  Block(pos, "images/objects/bonus_block/bonusblock.sprite"),
  m_is_triggered(false)
{
}

TrapBlock::TrapBlock(const ReaderMapping& reader) :
  Block(reader, "images/objects/bonus_block/bonusblock.sprite"),
  m_is_triggered(false)
{
}


void
TrapBlock::hit(Player& )
{
  if (m_is_triggered) return;

  m_is_triggered = true;
  start_bounce(nullptr);
  
  // Visual/Audio effects as per report
  SoundManager::current()->play("sounds/brick.wav", get_pos());
  Sector::get().add<Particles>(get_pos() + Vector(16, 16), 0.0f, 360.0f, 100.0f, 200.0f, Vector(0, 300), 10, Color(1.0f, 0, 0), 3, 1.0f, LAYER_OBJECTS + 1);

  spawn_random_badguy();
  
  // Make it look like an empty block after hit
  set_action("empty");
}

void
TrapBlock::spawn_random_badguy()
{
  std::vector<std::string> all_badguys = Sector::get().get_spawnable_badguys_list();
  if (all_badguys.empty()) return;

  // Filter out ONLY the specified badguys
  m_available_badguys.clear();
  for (const auto& name : all_badguys) {
    if (name == "stalactite" || name == "crusher" || name == "mrtree" ||
        name == "corrupted_granito_big" || name == "root_sapling" || 
        name == "skydive" || name == "tarantula" || name == "yeti" || 
        name == "ghosttree" || name == "yeti_stalactite" || name == "granito_giant") {
      continue;
    }
    m_available_badguys.push_back(name);
  }

  if (m_available_badguys.empty()) return;

  int index = graphicsRandom.rand(static_cast<int>(m_available_badguys.size()));
  std::string badguy_name = m_available_badguys[index];

  try {
    // Spawn 32 pixels above the block
    auto badguy = GameObjectFactory::instance().create(badguy_name, get_pos() + Vector(0, -32));
    BadGuy* bg = dynamic_cast<BadGuy*>(badguy.get());
    if (bg) {
       // Make it jump out a bit
       bg->get_physic().set_velocity_y(-300);
    }
    Sector::get().add_object(std::move(badguy));
  } catch (std::exception& e) {
    // log error if needed
  }
}




