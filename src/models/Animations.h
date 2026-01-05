#pragma once

#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <string>

#include "../util.h"

using NodeAnimMap = std::unordered_map<const aiNode *, const aiNodeAnim *>;

struct Animation
{
    NodeAnimMap node_anim_map;
    double ticks_per_second = 0, duration = 0;
};

class Animations
{
  public:
    Animations() = default;
    Animations(const aiScene *scene);

    bool set_animation(const std::string &name);

    Animation *get();

    void unset();

  private:
    std::unordered_map<std::string, Animation> animation_by_name;

    Animation *current_animation = 0;

    static std::string clean_name(const std::string &name);
};
