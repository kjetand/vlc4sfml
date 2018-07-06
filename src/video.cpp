#include "vlc4sfml/video.h"

#include <vlc/vlc.h>

struct Context
{
  unsigned char* frame{ nullptr };
};

namespace sf {

Video::Video() noexcept
{
  const char* vlc_args[] = { "--no-xlib", "--no-video-title-show" };
  constexpr auto num_args{ sizeof(vlc_args) / sizeof(*vlc_args) };

  m_vlc_instance = CustomUniquePtr<libvlc_instance_t>(
    libvlc_new(num_args, vlc_args), [](auto* ptr) { libvlc_release(ptr); });

  m_media_player = CustomUniquePtr<libvlc_media_player_t>(
    libvlc_media_player_new(m_vlc_instance.get()),
    [](auto* ptr) { libvlc_media_player_release(ptr); });
}

Video::Video(const std::string& video_file) noexcept
  : Video()
{
  load(video_file);
}

void
Video::load(const std::string& video_file) noexcept
{
  m_current_media = CustomUniquePtr<libvlc_media_t>(
    libvlc_media_new_path(m_vlc_instance.get(), video_file.c_str()),
    [](libvlc_media_t* ptr) { libvlc_media_release(ptr); });

  if (nullptr == m_current_media) {
    return;
  }
  libvlc_media_player_set_media(m_media_player.get(), m_current_media.get());

  const auto [width, height] = getResolution();

  // Allocate RGBA buffer for video frames
  m_frame.resize(width * height * 4);

  // Allocate SFML texture
  m_video_texture = sf::Texture();
  m_video_texture.create(width, height);
  m_video_sprite = sf::Sprite();
  m_video_sprite.setTexture(m_video_texture);

  // Make VLC fill raw buffer with video pixels
  libvlc_video_set_callbacks(
    m_media_player.get(), lock, unlock, display, m_frame.data());
  libvlc_video_set_format(
    m_media_player.get(), "RGBA", width, height, width * 4);

  // Set current size
  m_size = { width, height };
}

void
Video::play(const bool toggle) const noexcept
{
  libvlc_media_player_play(m_media_player.get());
}

void
Video::stop() const noexcept
{
  libvlc_media_player_stop(m_media_player.get());
}

void
Video::pause() const noexcept
{
  if (libvlc_media_player_can_pause(m_media_player.get()) != 0) {
    libvlc_media_player_set_pause(m_media_player.get(), 1);
  }
}

int
Video::getWidth() const noexcept
{
  return std::get<0>(m_size);
}

int
Video::getHeight() const noexcept
{
  return std::get<1>(m_size);
}

Size
Video::getSize() const noexcept
{
  return m_size;
}

int64_t
Video::getTime() const noexcept
{
  if (nullptr != m_current_media) {
    return libvlc_media_player_get_time(m_media_player.get());
  }
  return -1;
}

void
Video::seekTime(const int64_t micro_seconds) const noexcept
{
  if (nullptr != m_current_media) {
    libvlc_media_player_set_time(m_media_player.get(), micro_seconds);
  }
}

void
Video::skipTime(const int64_t micro_seconds) const noexcept
{
  if (nullptr != m_current_media) {
    const auto current_time =
      libvlc_media_player_get_time(m_media_player.get());
    const auto new_time = current_time + micro_seconds;

    libvlc_media_player_set_time(m_media_player.get(), new_time);
  }
}

int
Video::getVolume() const noexcept
{
  return libvlc_audio_get_volume(m_media_player.get());
}

void
Video::setVolume(const int volume) const noexcept
{
  libvlc_audio_set_volume(m_media_player.get(), volume);
}

void
Video::mute(const bool toggle) const noexcept
{
  // TODO: Toggle
  libvlc_audio_toggle_mute(m_media_player.get());
}

void
Video::render(RenderTarget& render_target) noexcept
{
  render(render_target, { 0, 0 }, m_size);
}

void
Video::render(RenderTarget& render_target, const Vector2f& position) noexcept
{
  render(render_target, position, m_size);
}

void
Video::render(RenderTarget& render_target,
              const Vector2f& position,
              const Size& size) noexcept
{
  m_video_texture.update(m_frame.data());
  m_video_sprite.setPosition(position);

  // Calculate scale
  const auto x_scale{ std::get<0>(size) / static_cast<float>(getWidth()) };
  const auto y_scale{ std::get<1>(size) / static_cast<float>(getHeight()) };

  m_video_sprite.setScale(x_scale, y_scale);
  render_target.draw(m_video_sprite);
}

Size
Video::getResolution() const noexcept
{
  if (nullptr == m_current_media) {
    return { 0, 0 };
  }

  // Disable deprecation warnings generated by LibVLC >= 3.0. Want to support
  // older VLC versions.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
  if (!libvlc_media_is_parsed(m_current_media.get())) {
    libvlc_media_parse(m_current_media.get());
  }
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

  unsigned int width{ 0 };
  unsigned int height{ 0 };

  libvlc_video_get_size(m_media_player.get(), 0, &width, &height);

  if (width == 0 || height == 0) {
    libvlc_media_track_t** info;
    const auto info_count =
      libvlc_media_tracks_get(m_current_media.get(), &info);

    for (unsigned int track_index = 0; track_index < info_count;
         ++track_index) {
      if (libvlc_track_video == info[track_index]->i_type &&
          info[track_index]->video->i_width &&
          info[track_index]->video->i_height) {
        width = info[track_index]->video->i_width;
        height = info[track_index]->video->i_height;
        break;
      }
    }
    libvlc_media_tracks_release(info, info_count);
  }
  return { static_cast<int>(width), static_cast<int>(height) };
}

void*
Video::lock(void* data, void** pixels)
{
  struct Context* context = static_cast<struct Context*>(data);
  *pixels = context;
  return nullptr;
}

void
Video::unlock(void* data, void* id, void* const* pixels)
{}

void
Video::display(void* data, void* id)
{}
}
