#pragma once
/**
 * Utilities for transforming matrices and vectors.
 */

class Matrix4x4;
class Vector3D;

Matrix4x4 xRotationMatrix(double rad);
Matrix4x4 yRotationMatrix(double rad);
Matrix4x4 zRotationMatrix(double rad);

Matrix4x4 scaleMatrix(double x, double y, double z);

Matrix4x4 translationMatrix(double x, double y, double z);

Matrix4x4 toZAxis(const Vector3D& unitV);

Vector3D reflect(const Vector3D& dir, const Vector3D& norm);
Vector3D refract(const Vector3D& in, const Vector3D& norm,
                 double ni, double nt);
