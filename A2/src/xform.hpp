#pragma once

#include "algebra.hpp"

Matrix4x4 xRotationMatrix(double rad);
Matrix4x4 yRotationMatrix(double rad);
Matrix4x4 zRotationMatrix(double rad);

Matrix4x4 scaleMatrix(double x, double y, double z);

Matrix4x4 translationMatrix(double x, double y, double z);
