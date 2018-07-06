# vlc4sfml
Video rendering support for SFML. The framework supports basic video operations: play, pause, stop, time control and sound control.

## Example
Rendering video into a SFML window:

```cpp
sf::RenderWindow window(...);
sf::Video video("/path/to/video.mp4");
video.play();

while (window.isOpen()) {
  window.clear();
  video.render(window);
  window.display();
}
```

## Status
First working implementation ready.

## License
Licensed under the [MIT License](LICENSE).
