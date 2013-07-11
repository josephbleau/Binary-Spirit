#include <iostream>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

namespace MiscUtils
{
	int max(int n1, int n2);
	int min(int n1, int n2);

	std::vector<std::string> explode(std::string str, std::string delim);
	bool is_flag_set(int flag, int& bits);
	void set_flag(int flag, int& bits);
	void unset_flag(int flag, int& bits);
	void fill_rect(sf::Image& img, const sf::FloatRect& rect, sf::Color color = sf::Color::Black);
}