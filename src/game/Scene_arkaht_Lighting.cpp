#include "Scene_arkaht_Lighting.h"

#include "../engine/Timer.h"

#include <assert.h>
#include <cstdlib>
#include <ctime>
#include <GL/glew.h>

void Scene_arkaht_Lighting::load()
{
    //  load shader
    shader = Assets::loadShader(
        "assets/shaders/arkaht_Lighting.vert", 
        "assets/shaders/arkaht_Lighting.frag", 
        "", "", "", 
        "arkaht_Lighting"
    );
    assert( shader.isValid() );
    
    //  load mesh
    mesh = new CubeMesh();
    mesh->load();

    //  create cubes
    for ( int i = 0; i < CUBES_COUNT; i++ )
    {
        cubes.emplace_back( 0.0f, 0.0f, mesh );
    }

    //  setup projection
    projection = Matrix4::createPerspectiveFOV( 
        70.0f, 
        game->windowWidth, 
        game->windowHeight, 
        0.1f, 
        1000.0f 
    );
}

void Scene_arkaht_Lighting::clean()
{
    //  release mesh
    mesh->clean();
    delete mesh;
}

void Scene_arkaht_Lighting::update( float dt )
{
    const float time = Timer::getTimeSinceStart() * 1.0f;
    const float angle_step = Maths::toRadians( 360.0f / CUBES_COUNT );

    for ( int i = 0; i < CUBES_COUNT; i++ )
    {
        CubeObject& cube = cubes[i];

        float radius = 1.5f + Maths::sin( i * 0.2f + time ) * 0.1f;
        float t = time + i * angle_step;

        cube.setPosition(
            Maths::cos( t ) * radius,
            Maths::sin( t ) * radius,
            -5.0f + Maths::cos( i * 1.5f + t ) * 5.0f
        );

        cube.setScale( Maths::cos( i + t ) * Maths::sin( i + t ) * 3.0f );
    }
}

void Scene_arkaht_Lighting::draw()
{
    //  clear screen
    static const GLfloat bgColor[] = { 0.0f, 0.0f, 0.2f, 1.0f };
    glClearBufferfv( GL_COLOR, 0, bgColor );

    //  send projection matrix
    shader.use();
    shader.setMatrix4( "proj_matrix", projection );

    for ( int i = 0; i < CUBES_COUNT; i++ )
    {
        CubeObject& cube = cubes[i];
        cube.draw( shader );
    }
}