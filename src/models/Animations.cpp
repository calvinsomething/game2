#include "Animations.h"
#include <cctype>

Animations::Animations(const aiScene *scene)
{
    for (size_t i = 0; i < scene->mNumAnimations; ++i)
    {
        aiAnimation *scene_anim = scene->mAnimations[i];

        std::string name = clean_name(scene_anim->mName.C_Str());
        animation_by_name[name] = {};

        Animation &animation = animation_by_name[name];
        animation.ticks_per_second = scene_anim->mTicksPerSecond;
        animation.duration = scene_anim->mDuration;

        for (size_t j = 0; j < scene_anim->mNumChannels; ++j)
        {
            aiNode *node = scene->mRootNode->FindNode(scene_anim->mChannels[j]->mNodeName);
            if (node)
            {
                animation.node_anim_map.insert({node, scene_anim->mChannels[j]});
            }
        }
    }
}

std::string Animations::clean_name(const std::string &name)
{
    size_t last_pipe = name.find_last_of("|");

    if (last_pipe == std::string::npos)
    {
        last_pipe = 0;
    }
    else
    {
        ++last_pipe;
    }

    std::string tail = name.substr(last_pipe);

    size_t start = tail.size() - 1, end = 0;

    for (size_t i = 0; i < tail.size(); ++i)
    {
        if (!std::isspace(tail[i]))
        {
            start = i < start ? i : start;
            end = i + 1 > end ? i + 1 : end;

            tail[i] = std::tolower(tail[i]);
        }
    }

    return tail.substr(start, end);
}

void Animations::set_looping(bool looping)
{
    loop = looping;
}

bool Animations::is_looping()
{
    return loop;
}

Animation *Animations::get()
{
    return current_animation;
}

void Animations::unset()
{
    current_animation = 0;
}

bool Animations::set_animation(const std::string &name)
{
    loop = false;

    auto p = animation_by_name.find(name);
    if (p != animation_by_name.end())
    {
        current_animation = &p->second;
        return true;
    }

    return false;
}
