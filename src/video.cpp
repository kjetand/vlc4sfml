#include "vlc4sfml/video.h"

#include <string>
#include <tuple>

namespace sf {

Video::Video(const std::string& video_file) {}

void
Video::load(const std::string& video_file) const noexcept
{}

void
Video::play(const bool toggle) const noexcept
{}

void
Video::stop() const noexcept
{}

void
Video::pause() const noexcept
{}

int
Video::getWidth() const noexcept
{
  return 0;
}

int
Video::getHeight() const noexcept
{
  return 0;
}

std::tuple<int, int>
Video::getSize() const noexcept
{
  return std::make_tuple(0, 0);
}

int64_t
Video::getTime() const noexcept
{
  return 0;
}

void
Video::seekTime(const int64_t micro_seconds) const noexcept
{}

void
Video::skipTime(const int64_t micro_seconds) const noexcept
{}

int
Video::getVolume() const noexcept
{
  return 0;
}

void
Video::setVolume(const int volume) const noexcept
{}

void
Video::mute(const bool toggle) const noexcept
{}
}
