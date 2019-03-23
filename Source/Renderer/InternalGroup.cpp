//  Copyright © 2019 Whelan Callahan. All rights reserved.
#include "InternalGroup.hpp"

InternalGroup::InternalGroup(uint64_t Parent) :
    parent(Parent),
    cameraAttachment(0),
    modelAttachment(0)
{}


InternalGroup::InternalGroup(const InternalGroup &g) :
    parent(g.parent),
    cameraAttachment(g.cameraAttachment),
    modelAttachment(g.modelAttachment),
    transform(g.transform),
    childGroups(g.childGroups)
{}


InternalGroup& InternalGroup::operator= (const InternalGroup &g)
{
    parent = g.parent;
    cameraAttachment = g.cameraAttachment;
    modelAttachment = g.modelAttachment;
    transform = g.transform;
    childGroups = g.childGroups;
    return *this;
}


InternalGroup::InternalGroup(InternalGroup &&g) :
    parent(g.parent),
    cameraAttachment(g.cameraAttachment),
    modelAttachment(g.modelAttachment),
    transform(g.transform),
    childGroups(std::move(g.childGroups))
{}


InternalGroup::~InternalGroup()
{}
