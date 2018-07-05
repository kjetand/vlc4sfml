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

  if (nullptr != m_current_media) {
    libvlc_media_player_set_media(m_media_player.get(), m_current_media.get());

    calculateResolution();

    // TODO: Implement rest of function
  }
}

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

void
Video::render(RenderTarget& render_target) const noexcept
{}

void
Video::render(RenderTarget& render_target, Vector2f& position) const noexcept
{}

void
Video::render(RenderTarget& render_target, Vector2f& position, Size& size) const
  noexcept
{}

void
Video::calculateResolution() noexcept
{
  if (nullptr == m_current_media) {
    m_size = { 0, 0 };
    return;
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
  m_size = { static_cast<int>(width), static_cast<int>(height) };
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
