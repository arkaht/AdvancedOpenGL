#include "Scene_arkaht_ParticleSystem.h"

#include "../engine/MacroUtils.h"

#include <assert.h>
#include <fstream>
#include <sstream>
#include <iostream>

void Scene_arkaht_ParticleSystem::load()
{
	srand( time( 0 ) );
	_time = rand();

	//  setup projection & view
	_projection = Matrix4::createPerspectiveFOV(
		70.0f,
		_game->windowWidth,
		_game->windowHeight,
		0.1f,
		1000.0f
	);
	_view_transform.set_position( Vector3 { 0.0f, 0.0f, -150.0f } );

	//  I need to enable this to use gl_PointSize in my vertex shader
	glEnable( GL_PROGRAM_POINT_SIZE );

	_initializeParticles();
	_initializeComputeShader();
	_initializeRenderShader();
}

void Scene_arkaht_ParticleSystem::clean()
{
}

void Scene_arkaht_ParticleSystem::handleEvent( const InputState& state )
{
	if ( state.keyboardState.isJustPressed( SDL_SCANCODE_R ) )
	{
		load();
	}

	if ( state.keyboardState.isDown( SDL_SCANCODE_T ) )
	{
		_sim.time_scale += 0.05f;
	}
	else if ( state.keyboardState.isDown( SDL_SCANCODE_G ) )
	{
		_sim.time_scale -= 0.05f;
	}
}

void Scene_arkaht_ParticleSystem::update( float dt )
{
	_delta_time = dt * _sim.time_scale;
	_time += _delta_time;

	float yaw = _time * 5.0f * _sim.blackhole_speed;
	float pitch = _time * 2.2f * _sim.blackhole_speed;
	float height = sinf( _time * 0.5f * _sim.blackhole_speed ) * _sim.blackhole_height;
	float dist = cosf( _time * _sim.blackhole_speed ) * _sim.blackhole_dist;

	_sim.blackhole1 = Vector3 
	{ 
		cosf( yaw ) * dist,
		cosf( pitch ) * -height,
		sinf( yaw ) * dist,
	};

	_sim.blackhole2 = Vector3 
	{ 
		cosf( yaw ) * -dist,
		cosf( pitch ) * height,
		sinf( yaw ) * -dist,
	};
}

void Scene_arkaht_ParticleSystem::draw()
{
	//  clear screen
	static const GLfloat bgColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glClearBufferfv( GL_COLOR, 0, bgColor );

	//  compute
	_compute_shader.use();
	_compute_shader.setFloat( "dt", _delta_time );
	_compute_shader.setFloat( "acceleration_force", _sim.acceleration_force );
	_compute_shader.setVector3f( "low_speed_color", _sim.low_speed_color );
	_compute_shader.setVector3f( "high_speed_color", _sim.high_speed_color );
	_compute_shader.setFloat( "high_speed_max", _sim.high_speed_max );
	_compute_shader.setVector3f( "blackhole1", _sim.blackhole1 );
	_compute_shader.setVector3f( "blackhole2", _sim.blackhole2 );
	_compute_shader.setVector3f( "blackhole_color", _sim.blackhole_color );
	_compute_shader.setFloat( "blackhole_color_dist_max", _sim.blackhole_color_dist_max );
	_compute_shader.setInteger( "in_sphere_enabled", _sim.in_sphere_enabled );
	_compute_shader.setFloat( "in_sphere_radius", _sim.in_sphere_radius );
	_compute_shader.setFloat( "in_sphere_bounciness", _sim.in_sphere_bounciness );
	glDispatchCompute( _particles_count / _work_group_size, 1, 1 );

	//  render
	_render_shader.use();
	_render_shader.setMatrix4( "projection", _projection );
	_render_shader.setMatrix4( "view", _view_transform.get_matrix() );
	_render_shader.setFloat( "point_size", _point_size );
	glDrawArrays( GL_POINTS, 0, _particles_count );
}

float Scene_arkaht_ParticleSystem::_getRandomInRange( float min, float max )
{
	return min + (float) ( rand() ) / ( (float) ( RAND_MAX / ( max - min ) ) );;
}

Vector3 Scene_arkaht_ParticleSystem::_getRandomPositionInSphere()
{
	Vector3 pos {
		_getRandomInRange( -1.0f, 1.0f ),
		_getRandomInRange( -1.0f, 1.0f ),
		_getRandomInRange( -1.0f, 1.0f )
	};
	pos.normalize();

	return pos;
}

void Scene_arkaht_ParticleSystem::_initializeParticles()
{
	std::vector<Vector4> positions( _particles_count );
	std::vector<Vector4> velocities( _particles_count );
	std::vector<Vector4> colors( _particles_count );

	for ( int i = 0; i < _particles_count; i++ )
	{
		positions[i] = Vector4 { _getRandomPositionInSphere() * 50.0f, 1.0f };
		velocities[i] = 
			//Vector4 { _getRandomPositionInSphere(), 0.0f };
			Vector4 {};
		colors[i] = Vector4 { 1.0f, 1.0f, 1.0f, 1.0f };
	}

	_createSSBOs( positions, velocities, colors );
}

void Scene_arkaht_ParticleSystem::_initializeComputeShader()
{
	//  read source file
	std::ifstream shader_reader( SHADER_COMP( arkaht_ParticleSystem ) );
    std::stringstream shader_stream;
	shader_stream << shader_reader.rdbuf();
	shader_reader.close();
	std::string source_code_str = shader_stream.str();
	const GLchar* source_code = source_code_str.c_str();

	_compute_shader_id = glCreateShader( GL_COMPUTE_SHADER );
	glShaderSource( _compute_shader_id, 1, &source_code, NULL );
	glCompileShader( _compute_shader_id );
	Shader::checkShaderErrors( _compute_shader_id, "compute" );

	_compute_program = glCreateProgram();
	glAttachShader( _compute_program, _compute_shader_id );
	glLinkProgram( _compute_program );

	glDeleteShader( _compute_shader_id );

	_compute_shader.id = _compute_program;
	assert( _compute_shader.isValid() );
}

void Scene_arkaht_ParticleSystem::_initializeRenderShader()
{
	_render_shader = Assets::loadShader(
		SHADER_VERT( arkaht_ParticleSystem ),
		SHADER_FRAG( arkaht_ParticleSystem ),
		"",
		"",
		"",
		"arkaht_ParticleSystem"
	);
	assert( _render_shader.isValid() );

	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, _ssbo_positions );
	glVertexPointer( 4, GL_FLOAT, 0, (void*) 0 );
	glEnableVertexAttribArray( 1 );
	glBindBuffer( GL_ARRAY_BUFFER, _ssbo_velocities );
	glVertexPointer( 4, GL_FLOAT, 0, (void*) 0 );
	glEnableVertexAttribArray( 2 );
	glBindBuffer( GL_ARRAY_BUFFER, _ssbo_colors );
	glVertexPointer( 4, GL_FLOAT, 0, (void*) 0 );

	glGenVertexArrays( 1, &_vao );
	glBindVertexArray( _vao );
}

void Scene_arkaht_ParticleSystem::_createSSBOs(
	const std::vector<Vector4>& positions,
	const std::vector<Vector4>& velocities,
	const std::vector<Vector4>& colors
)
{
	//  https://www.khronos.org/opengl/wiki/Shader_Storage_Buffer_Object

	//  positions
	glGenBuffers( 1, &_ssbo_positions );
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, _ssbo_positions );
	glBufferData( GL_SHADER_STORAGE_BUFFER,
		positions.size() * sizeof( Vector4 ),
		positions.data(),
		GL_STATIC_DRAW
	);

	//  velocities
	glGenBuffers( 1, &_ssbo_velocities );
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, _ssbo_velocities );
	glBufferData( GL_SHADER_STORAGE_BUFFER,
		velocities.size() * sizeof( Vector4 ),
		velocities.data(),
		GL_STATIC_DRAW
	);

	//  colors
	glGenBuffers( 1, &_ssbo_colors );
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, _ssbo_colors );
	glBufferData( GL_SHADER_STORAGE_BUFFER,
		colors.size() * sizeof( Vector4 ),
		colors.data(),
		GL_STATIC_DRAW
	);

	//  bind buffers bases
	glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 3, _ssbo_positions );
	glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 4, _ssbo_velocities );
	glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 5, _ssbo_colors );

	glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

	//  unbind buffer
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 );
}
