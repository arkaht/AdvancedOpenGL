#pragma once

#include "../engine/Scene.h"
#include "../engine/Assets.h"

#include "CubeMesh.h"

#include "Transform.hpp"

class Scene_arkaht_Tesselation : public Scene
{
public:
	~Scene_arkaht_Tesselation() { clean(); }

	void setGame( Game* game ) { _game = game; }

	void load();
	void clean();

	void pause() {}
	void resume() {}
	void handleEvent( const InputState& state );

	void update( float dt );
	void draw();

private:
	Game* _game { nullptr };
	Shader _shader;

	float _time = 0.0f;

	Transform _transform;
	CubeMesh _mesh;

	Matrix4 _projection;
};

