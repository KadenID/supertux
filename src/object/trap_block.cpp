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
TrapBlock::hit(Player& player)
{
  // 한 번 작동된 트랩 블록은 더 이상 작동하지 않음
  if (m_is_triggered) return;

  m_is_triggered = true;
  start_bounce(&player); // 블록이 튀어오르는 애니메이션 효과
  
  // 시각 및 청각 효과 처리 (벽돌 깨지는 소리와 붉은색 파티클 이펙트)
  SoundManager::current()->play("sounds/brick.wav", get_pos());
  Sector::get().add<Particles>(get_pos() + Vector(16, 16), 0.0f, 360.0f, 100.0f, 200.0f, Vector(0, 300), 10, Color(1.0f, 0, 0), 3, 1.0f, LAYER_OBJECTS + 1);

  // 스폰 방향 결정
  bool downward = false;
  if (player.m_does_buttjump) {
    // 블록 아래쪽 공간이 비어있는지 확인
    Rectf space_below;
    space_below.set_left(m_col.m_bbox.get_left() + 1);
    space_below.set_top(m_col.m_bbox.get_bottom() + 1);
    space_below.set_right(m_col.m_bbox.get_right() - 1);
    space_below.set_bottom(space_below.get_top() + 30);

    if (Sector::get().is_free_of_statics(space_below, this, true)) {
      downward = true;
    }
  }

  // 결정된 방향으로 무작위 적 스폰
  spawn_random_badguy(downward);
  
  // 타격 후 비어있는 블록의 모습으로 변경
  set_action("empty");
}

HitResponse
TrapBlock::collision(MovingObject& other, const CollisionHit& hit_)
{
  if (hit_.has_direction()) {
    auto player = dynamic_cast<Player*> (&other);
    if (player) {
      // 일반 보너스 블록과 동일하게 큰 Tux가 위에서 내려찍을 때(buttjump) 작동
      if (player->m_does_buttjump)
      {
        hit(*player);
      }
    }
  }

  return Block::collision(other, hit_);
}

void
TrapBlock::spawn_random_badguy(bool downward)
{
  // 현재 섹터에 스폰 가능한 모든 적들의 목록을 가져옴
  std::vector<std::string> all_badguys = Sector::get().get_spawnable_badguys_list();
  if (all_badguys.empty()) return;

  // 천장에서 떨어지는 적, 고정된 적, 보스 등 부적절한 적들을 필터링
  m_available_badguys.clear();
  for (const auto& name : all_badguys) {
    if (name == "stalactite" || name == "crusher" || name == "mrtree" || name == "dispenser" ||
        name == "corrupted_granito_big" || name == "root_sapling" || name == "owl" ||
        name == "skydive" || name == "tarantula" || name == "yeti" || 
        name == "ghosttree" || name == "yeti_stalactite" || name == "granito_giant") {
      continue;
    }
    m_available_badguys.push_back(name);
  }

  // 필터링 후 남은 적이 없다면 소환하지 않음
  if (m_available_badguys.empty()) return;

  // 남은 목록 중 무작위로 하나의 적을 선택
  int index = graphicsRandom.rand(static_cast<int>(m_available_badguys.size()));
  std::string badguy_name = m_available_badguys[index];

  try {
    // 스폰 위치 및 초기 속도 설정
    Vector spawn_pos = get_pos() + (downward ? Vector(0, 32) : Vector(0, -32));
    float initial_vy = downward ? 300.0f : -300.0f;

    auto badguy = GameObjectFactory::instance().create(badguy_name, spawn_pos);
    BadGuy* bg = dynamic_cast<BadGuy*>(badguy.get());
    if (bg) {
       // 적이 선택된 방향으로 튀어나오도록 초기 수직 속도 설정
       bg->get_physic().set_velocity_y(initial_vy);
    }
    Sector::get().add_object(std::move(badguy));
  } catch (std::exception& e) {
    // log error if needed
  }
}