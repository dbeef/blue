#pragma once

#include "blue/gpu/GpuCommandSystem.hpp"

void handle(UpdateEnvironmentEntity_CameraPos& entity);

void handle(UpdateEnvironmentEntity_LightPos& entity);

void handle(UpdateEnvironmentEntity_Projection& entity);

void handle(UpdateEnvironmentEntity_LightSpaceMatrix& entity);

void handle(UpdateEnvironmentEntity_View& entity);

void handle(UpdateEnvironmentEntity_AmbientStrength& entity);

void handle(UpdateEnvironmentEntity_LightColor& entity);
