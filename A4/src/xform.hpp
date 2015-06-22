// William McDonald 20418145 wmcdonal
#pragma once

class Matrix4x4;

Matrix4x4 xRotationMatrix(double rad);
Matrix4x4 yRotationMatrix(double rad);
Matrix4x4 zRotationMatrix(double rad);

Matrix4x4 scaleMatrix(double x, double y, double z);

Matrix4x4 translationMatrix(double x, double y, double z);
