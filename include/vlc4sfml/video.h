#pragma once

#include <string>
#include <tuple>

namespace sf {

class Video
{
public:
  explicit Video() = default;

  explicit Video(const std::string& video_file);

  ~Video() = default;

  void load(const std::string& video_file) const noexcept;

  void play(const bool toggle = false) const noexcept;

  void stop() const noexcept;

  void pause() const noexcept;

  int getWidth() const noexcept;

  int getHeight() const noexcept;

  std::tuple<int, int> getSize() const noexcept;

  int64_t getTime() const noexcept;

  void seekTime(const int64_t micro_seconds) const noexcept;

  void skipTime(const int64_t micro_seconds) const noexcept;

  int getVolume() const noexcept;

  void setVolume(const int volume) const noexcept;

  void mute(const bool toggle = false) const noexcept;
};
}
