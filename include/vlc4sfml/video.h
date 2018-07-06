#pragma once

#include <functional>
#include <memory>
#include <string>
#include <tuple>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

struct libvlc_instance_t;
struct libvlc_media_player_t;
struct libvlc_media_t;

namespace sf {

using Size = std::tuple<int, int>;

class Video
{
  template<typename T>
  using CustomUniquePtr = std::unique_ptr<T, std::function<void(T*)>>;

  CustomUniquePtr<libvlc_instance_t> m_vlc_instance{ nullptr };
  CustomUniquePtr<libvlc_media_player_t> m_media_player{ nullptr };
  CustomUniquePtr<libvlc_media_t> m_current_media{ nullptr };

  sf::Texture m_video_texture;
  sf::Sprite m_video_sprite;

  Size m_size{ 0, 0 };
  std::vector<unsigned char> m_frame;

public:
  explicit Video() noexcept;

  explicit Video(const std::string& video_file) noexcept;

  ~Video() = default;

  void load(const std::string& video_file) noexcept;

  void play(const bool toggle = false) const noexcept;

  void stop() const noexcept;

  void pause() const noexcept;

  int getWidth() const noexcept;

  int getHeight() const noexcept;

  Size getSize() const noexcept;

  int64_t getTime() const noexcept;

  void seekTime(const int64_t micro_seconds) const noexcept;

  void skipTime(const int64_t micro_seconds) const noexcept;

  int getVolume() const noexcept;

  void setVolume(const int volume) const noexcept;

  void mute(const bool toggle = false) const noexcept;

  void render(RenderTarget& render_target) noexcept;

  void render(RenderTarget& render_target, const Vector2f& position) noexcept;

  void render(RenderTarget& render_target,
              const Vector2f& position,
              const Size& size) noexcept;

private:
  Size getResolution() const noexcept;

  static void* lock(void* data, void** pixels);

  static void unlock(void* data, void* id, void* const* pixels);

  static void display(void* data, void* id);
};
}
