#pragma once

#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <string>

#include "../util.h"

using NodeAnimMap = StdUnorderedMap<const aiNode *, const aiNodeAnim *>;

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

    void set_looping(bool looping);
    bool is_looping();

    Animation *get();

    void unset();

  private:
    StdUnorderedMap<std::string, Animation> animation_by_name;

    Animation *current_animation = 0;

    static std::string clean_name(const std::string &name);

    bool loop = false;
};
