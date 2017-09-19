#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <twitter.h>
#include <random>
#include <chrono>
#include <thread>
#include <fstream>
#include <yaml-cpp/yaml.h>

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::cout << "usage: pokepronouns [configfile]" << std::endl;
    return -1;
  }

  std::string configfile(argv[1]);
  YAML::Node config = YAML::LoadFile(configfile);

  twitter::auth auth;
  auth.setConsumerKey(config["consumer_key"].as<std::string>());
  auth.setConsumerSecret(config["consumer_secret"].as<std::string>());
  auth.setAccessKey(config["access_key"].as<std::string>());
  auth.setAccessSecret(config["access_secret"].as<std::string>());

  twitter::client client(auth);

  std::vector<std::string> pokemon;
  std::map<std::string, std::string> genders;
  std::ifstream datafile(config["data"].as<std::string>());
  if (!datafile.is_open())
  {
    std::cout << "Could not find data file." << std::endl;
    return 1;
  }

  bool newgroup = true;
  std::string line;
  std::string curgroup;
  while (getline(datafile, line))
  {
    if (line.back() == '\r')
    {
      line.pop_back();
    }

    int divider = line.find_first_of(",");
    std::string name = line.substr(0, divider);
    std::string gender = line.substr(divider + 2);

    pokemon.push_back(name);
    genders[name] = gender;
  }

  std::random_device randomDevice;
  std::mt19937 rng{randomDevice()};
  std::uniform_int_distribution<int> dist(0, pokemon.size() - 1);

  for (;;)
  {
    std::cout << "Generating tweet" << std::endl;

    std::string choice = pokemon[dist(rng)];
    std::string gender = genders[choice];

    std::string result;
    if (gender == "both")
    {
      result = "Because not all {POKE} are male, it is incorrect to refer to a generic {POKE} as \"he\". Only a specific {POKE} may be a \"he\".";
    } else if (gender == "male")
    {
      result = "As there are multiple {POKE}, it is incorrect to refer to a generic one as \"he\". It is acceptable to refer to a specific one as such.";
    } else if (gender == "female")
    {
      result = "As there are multiple {POKE}, it is incorrect to refer to a generic one as \"she\". It is acceptable to refer to a specific one as such.";
    } else if (gender == "male legend")
    {
      result = "Because {POKE} is legendary and gendered, it is acceptable to refer to it as \"he\".";
    } else if (gender == "female legend")
    {
      result = "Because {POKE} is legendary and gendered, it is acceptable to refer to it as \"she\".";
    } else if (gender == "genderless")
    {
      result = "{POKE} is genderless. It is incorrect to ever refer to it as \"he\" or \"she\".";
    }

    int tknloc;
    while ((tknloc = result.find("{POKE}")) != std::string::npos)
    {
      std::string token = result.substr(tknloc+1, result.find("}")-tknloc-1);

      result.replace(tknloc, result.find("}")-tknloc+1, choice);
    }

    result.resize(140);

    try
    {
      client.updateStatus(result);
    } catch (const twitter::twitter_error& e)
    {
      std::cout << "Twitter error: " << e.what() << std::endl;
    }

    std::cout << result << std::endl;
    std::cout << "Waiting..." << std::endl;

    std::this_thread::sleep_for(std::chrono::hours(11));

    std::cout << std::endl;
  }
}
