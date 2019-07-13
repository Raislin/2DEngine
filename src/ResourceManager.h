#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "Types.h"
#include "Utilities.h"
#include <fstream>
#include <memory>
#include <sstream>
#include <unordered_map>

template <typename Derived, typename T>
class ResourceManager {

public:
    ResourceManager(const FilePath& config_file)
        : config_file_(config_file)
    {
        LoadPaths();
    }
    virtual ~ResourceManager()
    {
        resource_cache_.clear();
        paths_.clear();
    }

    std::shared_ptr<const T> GetResource(const ResourceName& id)
    {
        auto resource = Find(id).lock();

        if (!resource) {
            resource = RequireResource(id);
        }

        return resource;
    }

    const FilePath GetPath(const ResourceName& id) const
    {
        auto path = paths_.find(id);
        return (path != paths_.end() ? path->second : "");
    }

    void ReloadResourceConfiguration() { LoadPaths(); }

    void Update()
    {
        for (auto i = resource_cache_.begin(); i != resource_cache_.end();) {
            i = (i->second.expired()) ? resource_cache_.erase(i) : std::next(i);
        }
    }

private:
    std::shared_ptr<const T> LoadResource(const FilePath& path)
    {
        return static_cast<Derived*>(this)->LoadResource(path);
    }

    std::shared_ptr<const T> RequireResource(const ResourceName& id)
    {
        auto path = paths_.find(id);
        if (path == paths_.end()) {
            return nullptr;
        }
        auto resource = LoadResource(path->second);
        resource_cache_.emplace(id, resource);
        return resource;
    }

    std::weak_ptr<const T> Find(const ResourceName id)
    {
        auto itr = resource_cache_.find(id);
        return (itr != resource_cache_.end() ? itr->second : std::weak_ptr<const T>());
    }

    void LoadPaths()
    {
        std::ifstream paths_file(Utils::GetExecutableDirectory() + config_file_);
        if (paths_file.is_open()) {
            std::string line;
            while (std::getline(paths_file, line)) {
                std::stringstream keystream(line);
                std::string resource_id;
                std::string resource_path;
                keystream >> resource_id >> resource_path;
                paths_.emplace(resource_id, resource_path);
            }
            paths_file.close();
            return;
        }
        std::cerr << "! Failed loading the path file: " << config_file_
                  << std::endl;
    }

    FilePath config_file_;
    std::unordered_map<ResourceName, std::weak_ptr<const T>> resource_cache_;
    std::unordered_map<ResourceName, FilePath> paths_;
};

#endif
