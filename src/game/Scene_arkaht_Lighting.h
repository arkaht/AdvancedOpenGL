#pragma once

#include "../engine/Scene.h"
#include "../engine/Assets.h"

#include "CubeObject.h"
#include "CubeMesh.h"

class Scene_arkaht_Lighting : public Scene
{
public:
	Scene_arkaht_Lighting() {}
	~Scene_arkaht_Lighting() { clean(); }

	void setGame( Game* _game ) { game = _game; }

	void load();
	void clean();

	void pause() {}
	void resume() {}
	void handleEvent( const InputState& state ) {}

	void update( float dt );
	void draw();

private:
	Game* game { nullptr };
	Shader shader;

	CubeMesh* mesh { nullptr };
	const int CUBES_COUNT = 256;
	std::vector<CubeObject> cubes;


	Matrix4 projection;
};