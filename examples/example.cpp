#include <iostream>
#include <vlc4sfml.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Window/Event.hpp>

void
showVideo(sf::Video& video)
{
  sf::RenderWindow window(
    sf::VideoMode(800, 500), "vlc4sfml", sf::Style::Close);

  while (window.isOpen()) {
    sf::Event event;

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }
    window.clear();

    video.render(window, { 0, 0 }, { 400, 250 });
    video.render(window, { 400, 0 }, { 400, 250 });
    video.render(window, { 0, 250 }, { 400, 250 });
    video.render(window, { 400, 250 }, { 400, 250 });

    window.display();

    sf::sleep(sf::milliseconds(10));
  }
}

int
main(int argc, char** argv)
{
  if (argc != 2) {
    std::cout << "Usage: ./vlc4sfml-example <filename>" << std::endl;
    return 1;
  }
  sf::Video video{ std::string(argv[1]) };
  video.play();

  showVideo(video);

  return 0;
}
