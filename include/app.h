#pragma once

#include <memory>
#include <set>
#include <vector>
#include "glapplication.h"
#include "pipeline.h"
#include "assetfactory.h"
#include "glgamestate.h"
#include "playercontrol.h"
#include "rotationgraph.h"
#include "ex/gltextrenderer.h"

enum class GameStateType
{
    MAIN
};

using TextureArray = std::vector<std::shared_ptr<lithium::Texture<unsigned char>>>;

class App : public lithium::Application
{
public:
    App();

    virtual ~App() noexcept;

    virtual void update(float dt) override;

    void addStateTransition(GameStateType gs)
    {
        if(gs != _currentGameState)
        {
            _stateTransitions.emplace(gs);
        }
    }

    void handleStateTransitions();

    std::map<GameStateType, lithium::GameState>::iterator currentGameState()
    {
        return  _gameStates.find(_currentGameState);
    }


private:

    void constructRotationGraph(std::vector<lithium::Node*>& nodes);

    void createLevel1();

    std::shared_ptr<Pipeline> _pipeline{nullptr};
    std::set<GameStateType> _stateTransitions;
    std::map<GameStateType, lithium::GameState> _gameStates;
    GameStateType _currentGameState;
    std::unique_ptr<PlayerControl> _playerControl;
    RotationGraph _rotationGraph;
    float _cameraViewAngle{glm::pi<float>() * 0.25f};
    lithium::ExTextRenderer _textRenderer;
};
