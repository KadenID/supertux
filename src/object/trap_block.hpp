//  SuperTux
//  Copyright (C) 2024
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.

#pragma once

#include "object/block.hpp"
#include <vector>
#include <string>

class ReaderMapping;

class TrapBlock final : public Block
{
public:
  TrapBlock(const Vector& pos);
  TrapBlock(const ReaderMapping& reader);

  // 플레이어가 아래에서 블록을 쳤을 때 호출되는 메서드
  // 일반적인 아이템 대신 적을 소환.
  virtual void hit(Player& player) override;
  
  static std::string class_name() { return "trapblock"; }
  static const char* display_name() { return "Trap Block"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual GameObjectClasses get_class_types() const override { return Block::get_class_types().add(typeid(TrapBlock)); }

private:
  // 스폰 가능한 적 목록 중 특정 적들을 제외한 무작위 적을 소환하는 헬퍼 메서드
  void spawn_random_badguy();

private:
  std::vector<std::string> m_available_badguys; // 소환 가능한 적들의 이름 목록
  bool m_is_triggered; // 블록이 이미 작동했는지 확인하는 플래그 (한 번만 작동하도록)
};
