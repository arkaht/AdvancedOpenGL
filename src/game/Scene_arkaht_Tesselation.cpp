#include "Scene_arkaht_Tesselation.h"

#include "../engine/MacroUtils.h"

#include <assert.h>

void Scene_arkaht_Tesselation::load()
{
	_shader = Assets::loadShader( 
		SHADER_VERT( arkaht_Tesselation ),
		SHADER_FRAG( arkaht_Tesselation ),
		SHADER_TECS( arkaht_Tesselation ),
		SHADER_TESE( arkaht_Tesselation ),
		"",//SHADER_GEOM( arkaht_Tesselation ),
		"arkaht_Tesselation"
	);
	assert( _shader.isValid() );

	//  load mesh
	_mesh.load();
	glPatchParameteri( GL_PATCH_VERTICES, 3 );

	//  enable culling
	glEnable( GL_CULL_FACE );
	glFrontFace( GL_CW );

	//  setup projection
	_projection = Matrix4::createPerspectiveFOV(
		70.0f,
		_game->windowWidth,
		_game->windowHeight,
		0.1f,
		1000.0f
	);

	//  setup transform
	_transform.set_position( Vector3 { 0.0f, 0.0f, -4.0f } );
	_transform.set_scale( Vector3 { 2.0f, 2.0f, 2.0f } );
}

void Scene_arkaht_Tesselation::clean()
{
	_mesh.clean();
}

void Scene_arkaht_Tesselation::handleEvent( const InputState& state )
{
	Vector2 pos = state.mouseState.getPosition();
	//  get as screen ratio, in range [-0.5; 0.5]
	_mouse_position.x = pos.x / _game->windowWidth;
	_mouse_position.y = pos.y / _game->windowHeight;
	//  get normalized, in range [-1.0; 1.0]
	_mouse_position = 2.0f * _mouse_position;

	//printf( "x=%f; y=%f\n", _mouse_position.x, _mouse_position.y );

	_time_scale_target = state.mouseState.getButtonValue( 1 ) ? 0.2f : 1.0f;
}

void Scene_arkaht_Tesselation::update( float dt )
{
	//  manipulate time
	_time_scale = Maths::lerp( _time_scale, _time_scale_target, dt * 5.0f );
	dt *= _time_scale;
	_time += dt;

	//  translate
	/*_transform.set_position( 
		Vector3 { 
			Maths::cos( _time ), 
			Maths::sin( _time ) * 0.2f,
			-4.0f 
		} 
	);*/

	//  rotate
	Vector3 angles_speed {
		Maths::cos( _time ) * dt * 5.0f,
		Maths::sin( _time ) * dt * 2.0f,
		Maths::cos( _time * 2.0f ) * dt,
	};
	_transform.set_angles( _transform.get_angles() + angles_speed );

	/*Quaternion rotation = _transform.get_rotation();
	rotation = Quaternion::concatenate( 
		rotation, 
		Quaternion( Vector3::unitX, angles_speed.x ) 
	);
	rotation = Quaternion::concatenate(
		rotation,
		Quaternion( Vector3::unitY, angles_speed.y )
	);
	rotation = Quaternion::concatenate(
		rotation,
		Quaternion( Vector3::unitZ, angles_speed.z )
	);
	_transform.set_rotation( rotation );*/
	
	//  scale
	/*float scale = 1.0f + Maths::abs( Maths::cos( _time ) );
	_transform.set_scale( Vector3 { scale, scale, scale } );*/
}

void Scene_arkaht_Tesselation::draw()
{
	//  clear screen
	static const GLfloat bgColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glClearBufferfv( GL_COLOR, 0, bgColor );

	//  draw mesh
	_shader.use();
	_shader.setMatrix4( "projection", _projection );
	_shader.setMatrix4( "model", _transform.get_matrix() );
	_shader.setVector2f( "mouse", _mouse_position );
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glPointSize( 5.0f );
	glDrawArrays( GL_PATCHES, 0, 36 );
}
