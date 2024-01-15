#pragma once

#include "../engine/Scene.h"
#include "../engine/Assets.h"

#include "Transform.hpp"

class Scene_arkaht_ParticleSystem : public Scene
{
public:
	~Scene_arkaht_ParticleSystem() { clean(); }

	void setGame( Game* game ) { _game = game; }

	void load();
	void clean();

	void pause() {}
	void resume() {}
	void handleEvent( const InputState& state );

	void update( float dt );
	void draw();

private:
	float _getRandomInRange( float min, float max );
	Vector3 _getRandomPositionInSphere();

	void _initializeParticles();
	void _initializeComputeShader();
	void _initializeRenderShader();

	void _createSSBOs( 
		const std::vector<Vector4>& positions,
		const std::vector<Vector4>& velocities,
		const std::vector<Vector4>& colors
	);

private:
	const int _particles_count = 5000000;
	const int _work_group_size = 100;
	const float _point_size = 20.0f;

private:
	Game* _game { nullptr };
	Shader _render_shader;
	Shader _compute_shader;

	GLuint _compute_shader_id;
	GLuint _compute_program;

	GLuint _vao;
	GLuint _ssbo_positions;
	GLuint _ssbo_velocities;
	GLuint _ssbo_colors;

	float _time = 0.0f;
	float _delta_time = 0.0f;

	struct {
		float time_scale = 4.0f;
		float acceleration_force = 50.0f;

		Vector3 low_speed_color { 0.0f, 0.25f, 1.0f };
		Vector3 high_speed_color { 1.0f, 0.0f, 1.0f };
		float high_speed_max = 15.0f;

		Vector3 blackhole1 { 0.0f, 0.0f, 0.0f };  //  don't change, auto-updated
		Vector3 blackhole2 { 0.0f, 0.0f, 0.0f };  //  don't change, auto-updated
		Vector3 blackhole_color { 1.0f, 1.0f, 1.0f };
		float blackhole_speed = 0.03f;
		float blackhole_dist = 75.0f;
		float blackhole_height = 50.0f;
		float blackhole_color_dist_max = 20.0f;

		bool in_sphere_enabled = true;
		float in_sphere_radius = 50.0f;
		float in_sphere_bounciness = 0.2f;
	} _sim;

	Matrix4 _projection;
	Transform _view_transform;
};

