#include "Scene_arkaht_Lighting.h"

#include "../engine/Timer.h"

#include <assert.h>
#include <cstdlib>
#include <ctime>
#include <GL/glew.h>

void Scene_arkaht_Lighting::load()
{
	//  load shader
	_shader = Assets::loadShader(
		"assets/shaders/arkaht_Lighting.vert",
		"assets/shaders/arkaht_Lighting.frag",
		"", "", "",
		"arkaht_Lighting"
	);
	assert( _shader.isValid() );

	//  load mesh
	setup_mesh();

	//  create cubes
	for ( int i = 0; i < CUBES_COUNT; i++ )
	{
		_transforms.emplace_back();
	}

	//  setup projection
	_projection = Matrix4::createPerspectiveFOV(
		70.0f,
		_game->windowWidth,
		_game->windowHeight,
		0.1f,
		1000.0f
	);

	//  enable culling
	glEnable( GL_CULL_FACE );
	glFrontFace( GL_CW );

	//  enable depth test
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );

	//  setup light
	_light_transform.set_position( Vector3 { 0.0f, 0.0f, -5.0f } );
	_light_transform.set_scale( Vector3 { 0.5f, 0.5f, 0.5f } );
	_light_color = Vector4 { 1.0f, 1.0f, 1.0f, 1.0f };
	_light_ambient = 0.1f;
	_light_radius = 100.0f;
}

void Scene_arkaht_Lighting::clean()
{
	//  release mesh
	glDeleteBuffers( 1, &_mesh.vao );
	glDeleteBuffers( 1, &_mesh.vbo );
	glDeleteBuffers( 1, &_mesh.ibo );
}

void Scene_arkaht_Lighting::handleEvent( const InputState& state )
{
	//  LMB to slow motion
	_time_scale = state.mouseState.getButtonValue( 1 ) ? 0.2f : 1.0f;

	//  RMB to switch rendering mode
	if ( state.mouseState.getButtonState( 3 ) == KeyStatus::JustPressed )
	{
		_mesh.polygon_mode = _mesh.polygon_mode == GL_FILL ? GL_LINE : GL_FILL;
	}
}

void Scene_arkaht_Lighting::update( float dt )
{
	const float angle_step = Maths::toRadians( 360.0f / CUBES_COUNT );

	//  update time
	_time += dt * _time_scale;

	//  update light
	_light_transform.set_position(
		Vector3 {
			Maths::cos( _time * 0.5f ) * 3.0f,
			Maths::sin( _time * 0.5f ) * 3.0f,
			-5.0f
		}
	);
	//_light_ambient = Maths::abs( Maths::sin( _time ) * 0.2f );

	//  update cubes
	for ( int i = 0; i < CUBES_COUNT; i++ )
	{
		Transform& transf = _transforms[i];

		float radius = 2.0f + Maths::sin( i * 0.2f + _time ) * 0.1f;
		float t = _time + i * angle_step;

		//  translate
		transf.set_position(
			Vector3 {
				Maths::cos( t ) * radius,
				Maths::sin( t ) * radius,
				-5.0f + Maths::cos( i * 1.5f + t ) * 5.0f
			}
		);

		//  scale
		float scale = Maths::abs( Maths::cos( i + t ) * Maths::sin( i + t ) ) * 3.0f;
		transf.set_scale( Vector3( scale, scale, scale ) );
	}
}

void Scene_arkaht_Lighting::draw()
{
	//  clear screen
	static const GLfloat bgColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glClearBufferfv( GL_COLOR, 0, bgColor );

	//  setup shader
	_shader.use();
	_shader.setMatrix4( "projection", _projection );
	_shader.setVector3f( "light_position", _light_transform.get_position() );
	_shader.setVector4f( "light_color", _light_color );
	_shader.setFloat( "light_ambient", _light_ambient );
	_shader.setFloat( "light_radius", _light_radius );

	//  set draw mode
	glPolygonMode( GL_FRONT_AND_BACK, _mesh.polygon_mode );
	
	//  draw meshes
	for ( int i = 0; i < CUBES_COUNT; i++ )
	{
		Transform& transf = _transforms[i];
		draw_mesh( transf );
	}
	
	//  draw light
	draw_mesh( _light_transform, true );
}

void Scene_arkaht_Lighting::setup_mesh()
{
	//  setup vertices & indices
	const unsigned int vertex_elements_stride = 6;
	std::vector<float> vertices {
		/*
		 *  NOTE:
		 *  the normals of my cube are not face normals, they should
		 *  but I haven't found code which already do it and I didn't want
		 *  to lose my time finding them by myself and to re-setup all 
		 *  the vertices and indices..
		 * 
		 *  so this explains why I have a smooth light shading on my cubes
		 */

		//  0
		-0.5f, -0.5f,  0.5f,  //  position
		-1.0f, -1.0f, 1.0f,   //  normal

		//  1
		0.5f, -0.5f,  0.5f,   //  position
		1.0f, -1.0f, 1.0f,    //  normal

		//  2
		-0.5f,  0.5f,  0.5f,  //  position
		-1.0f, 1.0f, 1.0f,    //  normal

		//  3
		0.5f,  0.5f,  0.5f,   //  position
		1.0f, 1.0f, 1.0f,     //  normal

		//  4
		-0.5f, -0.5f, -0.5f,  //  position
		-1.0f, -1.0f, -1.0f,  //  normal

		//  5
		0.5f, -0.5f, -0.5f,   //  position
		1.0f, -1.0f, -1.0f,   //  normal

		//  6
		-0.5f,  0.5f, -0.5f,  //  position
		-1.0f, 1.0f, -1.0f,   //  normal

		//  7
		0.5f,  0.5f, -0.5f,   //  position
		1.0f, 1.0f, -1.0f,    //  normal
	};
	std::vector<unsigned int> indices {
		//Top
		2, 6, 7,
		2, 3, 7,

		//Bottom
		0, 4, 5,
		0, 1, 5,

		//Left
		0, 2, 6,
		0, 4, 6,

		//Right
		1, 3, 7,
		1, 5, 7,

		//Front
		0, 2, 3,
		0, 1, 3,

		//Back
		4, 6, 7,
		4, 5, 7
	};

	_mesh.n_indices = indices.size();
	_mesh.n_vertices = vertices.size() / vertex_elements_stride;
	printf( "mesh has %d indices and %d vertices\n", _mesh.n_indices, _mesh.n_vertices );

	//  create vao
	glGenVertexArrays( 1, &_mesh.vao );
	glBindVertexArray( _mesh.vao );

	//  create vbo
	glGenBuffers( 1, &_mesh.vbo );
	glBindBuffer( GL_ARRAY_BUFFER, _mesh.vbo );
	glBufferData(
		GL_ARRAY_BUFFER,
		_mesh.n_vertices * vertex_elements_stride * sizeof( float ),
		vertices.data(),
		GL_STATIC_DRAW
	);

	//  create ibo
	glGenBuffers( 1, &_mesh.ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _mesh.ibo );
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		_mesh.n_indices * sizeof( unsigned int ),
		indices.data(),
		GL_STATIC_DRAW
	);

	//  set vertex attributes
	glEnableVertexAttribArray( 0 );  //  position
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE,
		sizeof( float ) * vertex_elements_stride,
		0
	);
	glEnableVertexAttribArray( 1 );  //  normal
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE,
		sizeof( float ) * vertex_elements_stride,
		reinterpret_cast<void*>( sizeof( float ) * 3 )
	);
	//glEnableVertexAttribArray( 2 );  //  uv
	//glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 
	//	sizeof( float ) * vertex_elements_stride, 
	//	reinterpret_cast<void*>( sizeof( float ) * 6 ) 
	//);
}

void Scene_arkaht_Lighting::draw_mesh( Transform& transf, bool is_light )
{
	_shader.setMatrix4( "model", transf.get_matrix() );
	_shader.setInteger( "is_light", is_light );

	//  draw
	glBindVertexArray( _mesh.vao );
	glDrawElements(
		GL_TRIANGLE_STRIP,
		_mesh.n_indices,
		GL_UNSIGNED_INT,
		nullptr
	);
}
