#include "MiscUtils.h"

std::vector<std::string> MiscUtils::explode(std::string str, std::string delim)
{
  std::vector<std::string> fragments;
  size_t found = 0;
  size_t delim_sz = delim.size();

  while((found = str.find(delim)) != std::string::npos)
  {
    fragments.push_back(str.substr(0, found));
    str = str.substr(found+delim_sz);
  }

  fragments.push_back(str);

  return fragments;
}

bool MiscUtils::is_flag_set(int flag, int& bits)
{
  return (bits & (1 << flag)) != 0;
}

void MiscUtils::set_flag(int flag, int& bits)
{
  bits |= 1 << flag;
}

void MiscUtils::unset_flag(int flag, int& bits)
{
  bits &= ~(1 << flag); 
}

int MiscUtils::max(int n1, int n2)
{
  if(n1 >= n2)
  {
    return n1;
  }
  else
  {
    return n2;
  }
}

int MiscUtils::min(int n1, int n2)
{
  if(n1 <= n2)
  {
    return n1;
  }
  else
  {
    return n2;
  }
}

void MiscUtils::fill_rect(sf::Image& img, const sf::FloatRect& rect, sf::Color color)
{
  for(unsigned int x = (unsigned int) rect.left; x < rect.left + rect.width; x++)
  {
    for(unsigned int y = (unsigned int) rect.top; y< rect.top + rect.width; y++)
    {
      img.setPixel(x,y, color);
    }
  }
}