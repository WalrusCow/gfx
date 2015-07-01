#pragma once

#include <string>

#include "scene.hpp"
#include "RayTracer.hpp"

extern RayTracer::Options rayTracerOptions;

bool run_lua(const std::string& filename);
