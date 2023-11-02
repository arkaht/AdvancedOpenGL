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
}

void Scene_arkaht_Tesselation::clean()
{
	_mesh.clean();
}

void Scene_arkaht_Tesselation::handleEvent( const InputState& state )
{
}

void Scene_arkaht_Tesselation::update( float dt )
{
	_time += dt;
	_transform.set_position( Vector3 { Maths::cos( _time ), 0.0f, -4.0f } );
}

void Scene_arkaht_Tesselation::draw()
{
	_shader.use();
	_shader.setMatrix4( "projection", _projection );
	_shader.setMatrix4( "model", _transform.get_matrix() );
	_mesh.draw();
}
