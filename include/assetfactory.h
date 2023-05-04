#pragma once

#include "glfont.h"
#include "globject.h"
#include "glanimatedobject.h"
#include "gltinyobjloader.h"
#include "gltfloader.h"
#include "glskinnedobject.h"

class AssetFactory
{
public:
    struct Meshes
    {
        std::shared_ptr<lithium::Mesh> cube;
        std::shared_ptr<lithium::Mesh> screen;
        std::shared_ptr<lithium::Mesh> sprite;
        std::shared_ptr<lithium::Mesh> package;
        std::shared_ptr<lithium::Mesh> longPackage;
        std::shared_ptr<lithium::Mesh> slot;
        std::shared_ptr<lithium::Mesh> level1;
    } _meshes;

    struct Textures
    {
        std::shared_ptr<lithium::ImageTexture> logoDiffuse;
        std::shared_ptr<lithium::ImageTexture> delivermanSheet;
        std::shared_ptr<lithium::ImageTexture> packageDiffuse;
        std::shared_ptr<lithium::ImageTexture> letterDiffuse;
    } _textures;

    struct Objects
    {

    } _objects;

    struct Fonts
    {
        std::shared_ptr<lithium::Font> righteousFont;
    } _fonts;

    AssetFactory();
    virtual ~AssetFactory() noexcept;
    AssetFactory(AssetFactory const&) = delete;
    void operator=(AssetFactory const&)  = delete;

    static void loadMeshes();
    static void loadTextures();
    static void loadObjects();
    static void loadFonts();

    static AssetFactory& getInstance();

    static const std::vector<lithium::VertexArrayBuffer::AttributeType> objectAttributes;
    static const std::vector<lithium::VertexArrayBuffer::AttributeType> modelAttributes;

    static const AssetFactory::Meshes* getMeshes();
    static const AssetFactory::Textures* getTextures();
    static const AssetFactory::Objects* getObjects();
    static const AssetFactory::Fonts* getFonts();

    gltf::Loader _gltfLoader;
};