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

//! \brief A video class for SFML that renders video to a SFML render target.
//! Supports basic video operations like play, pause, stop, time control and
//! sound control.
//
//! Example:
//! \code{.cpp}
//! sf::RenderWindow window(...);
//! sf::Video video("/path/to/video.mp4");
//! video.play();
//!
//! while (window.isOpen()) {
//!   window.clear();
//!   video.renderTo(window);
//!   window.display();
//! }
//! \endcode
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

  bool m_is_playing{ false };

public:
  //! Initialize video internals.
  explicit Video() noexcept;

  //! Initialize video internals and load video file.
  explicit Video(const std::string& video_file) noexcept;

  //! Destructor.
  ~Video();

  //! Assign video file and allocate internal buffers for video rendering.
  void load(const std::string& video_file) noexcept;

  //! Play loaded video file. Supports toggling between play and pause. Toggle
  //! is by default enabled.
  void play(const bool toggle = true) noexcept;

  //! Stop video if playing.
  void stop() noexcept;

  //! Pause video. Supports toggling between pause and play. Toggle is by
  //! default disabled.
  void pause(const bool toggle = false) noexcept;

  //! Get number of horizontal pixels.
  int getWidth() const noexcept;

  //! Get number of verical pixels.
  int getHeight() const noexcept;

  //! Get video resolution.
  Size getSize() const noexcept;

  //! Get current time in micro seconds.
  int64_t getTime() const noexcept;

  //! Seek and set video to given time.
  void seekTime(const int64_t micro_seconds) const noexcept;

  //! Skip a given number of micro seconds from current time. Negative number
  //! will skip backwards.
  void skipTime(const int64_t micro_seconds) const noexcept;

  //! Returns volume as percent [0, 100].
  int getVolume() const noexcept;

  //! Sets volume where volume is represented as percent [0, 100].
  void setVolume(const int volume) const noexcept;

  //! Mutes the sound. Supports toggling which is enabled by default.
  void mute(const bool toggle = true) const noexcept;

  //! Renders current video frame to a render target with original resolution at
  //! position (0, 0).
  void renderTo(RenderTarget& render_target) noexcept;

  //! Renders current video frame to a render target with original resolution at
  //! a given position.
  void renderTo(RenderTarget& render_target, const Vector2f& position) noexcept;

  //! Renders current video frame to a render target with a given position and
  //! size in pixels.
  void renderTo(RenderTarget& render_target,
                const Vector2f& position,
                const Size& size) noexcept;

private:
  Size getResolution() const noexcept;

  struct Context
  {
    unsigned char* frame{ nullptr };
  };

  static void* lock(void* data, void** pixels);

  static void unlock(void* data, void* id, void* const* pixels);

  static void display(void* data, void* id);
};
}
