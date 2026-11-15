#include <SFML/Graphics.hpp>
using namespace sf;

int main()
{
	RenderWindow ventana(VideoMode(1400, 800),"Sistema de Aerolinea");
	
	while (ventana.isOpen())
	{
		Event evento;
		
		while (ventana.pollEvent(evento))
		{
			if (evento.type == Event::Closed)
			{
				ventana.close();
			}
		}
		
		ventana.clear(Color(20, 45, 80));
		
		CircleShape circulo(50);
		circulo.setFillColor(Color::White);
		circulo.setPosition(375, 275);
		
		ventana.draw(circulo);
		
		ventana.display();
	}
	
	return 0;
}
