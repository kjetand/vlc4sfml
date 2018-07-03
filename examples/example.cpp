#include <iostream>
#include <vlc4sfml.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Window/Event.hpp>

void
showVideo(const sf::Video& video)
{
  sf::RenderWindow window(sf::VideoMode(1000, 600), "vlc4sfml");

  while (window.isOpen()) {
    sf::Event event;

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::Resized) {
        const auto size{ window.getSize() };
        const sf::View view(sf::FloatRect(
          0.f, 0.f, static_cast<float>(size.x), static_cast<float>(size.y)));
        window.setView(view);
      }
    }
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
  showVideo(video);

  return 0;
}
