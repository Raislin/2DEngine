#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "ResourceManager.h"
#include "Types.h"
#include "Utilities.h"
#include <SFML/Graphics/Texture.hpp>
#include <iostream>

class TextureManager : public ResourceManager<TextureManager, const sf::Texture> {
public:
    TextureManager()
        : ResourceManager("resources/textures.cfg")
    {
    }

    std::shared_ptr<sf::Texture> LoadResource(FilePath texture_file)
    {
        auto texture = std::make_shared<sf::Texture>();
        if (!texture->loadFromFile(Utils::GetExecutableDirectory() + texture_file)) {
            std::cerr << "Texture failed to load: " << texture_file << "\n";
        }

        return texture;
    }
};

#endif // !TEXTURE_MANAGER_H
