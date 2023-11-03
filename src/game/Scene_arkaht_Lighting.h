#pragma once

#include "../engine/Scene.h"
#include "../engine/Assets.h"

#include "CubeObject.h"
#include "CubeMesh.h"

#include "Transform.hpp"

class Scene_arkaht_Lighting : public Scene
{
public:
	Scene_arkaht_Lighting() {}
	~Scene_arkaht_Lighting() { clean(); }

	void setGame( Game* game ) { _game = game; }

	void load();
	void clean();

	void pause() {}
	void resume() {}
	void handleEvent( const InputState& state );

	void update( float dt );
	void draw();

private:
	void setup_mesh();
	void draw_mesh( Transform& transf, bool is_light = false );

	Game* _game { nullptr };
	Shader _shader;

	struct {
		GLuint vao { 0 };
		GLuint vbo { 0 };
		GLuint ibo { 0 };
		GLuint n_indices { 0 };
		GLuint n_vertices { 0 };
		GLenum polygon_mode { GL_FILL };
	} _mesh;

	const int CUBES_COUNT = 64;
	std::vector<Transform> _transforms;

	float _time = 0.0f;
	float _time_scale = 1.0f;

	float _light_ambient;
	float _light_radius;
	Transform _light_transform;
	Vector4 _light_color;

	Matrix4 _projection;
};