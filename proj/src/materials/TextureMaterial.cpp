#include "TextureMaterial.hpp"

#include "lodepng.h"

#include "HitRecord.hpp"

TextureMaterial::TextureMaterial(
    const std::string& filename, const Colour& ks_, double shininess_)
    : Material(ks_, shininess_), texture(readTexture(filename)) {}

TextureMaterial::Texture TextureMaterial::readTexture(
    const std::string& filename) const {
  std::vector<unsigned char> image;

  Texture texture;
  unsigned error = lodepng::decode(
      image, texture.width, texture.height, filename);
  if (error) {
    std::cerr << "Error reading PNG: " << error << ": "
              << lodepng_error_text(error) << std::endl;
  }
  texture.data.reserve(image.size());

  // Four bytes at a time. RGBA.
  for (size_t i = 0; i < image.size(); i += 4) {
    texture.data.emplace_back(Colour(
          image[i] / 255.0, image[i + 1] / 255.0, image[i + 2] / 255.0));
  }

  return texture;
}

Colour TextureMaterial::getKd(const HitRecord& hitRecord) const {
  if (hitRecord.xPercent < 0 || hitRecord.yPercent < 0) return Colour(0, 0, 0);

  size_t xPx = (size_t) (hitRecord.xPercent * texture.width);
  size_t yPx = (size_t) (hitRecord.yPercent * texture.height);
  // Invert y part because model is bottom to top and image is top to bottom
  yPx = texture.height - yPx;
  return texture.data[(yPx * texture.width) + xPx];
}

