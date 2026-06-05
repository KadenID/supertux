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

  virtual void hit(Player& player) override;
  
  static std::string class_name() { return "trapblock"; }
  static const char* display_name() { return "Trap Block"; }
  virtual std::string get_class_name() const override { return class_name(); }
  virtual GameObjectClasses get_class_types() const override { return Block::get_class_types().add(typeid(TrapBlock)); }

private:
  void spawn_random_badguy();

private:
  std::vector<std::string> m_available_badguys;
  bool m_is_triggered;
};
