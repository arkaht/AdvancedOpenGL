#include "CubeObject.h"
#include "../engine/Shader.h"
#include "CubeMesh.h"

CubeObject::CubeObject( float xP, float yP, CubeMesh* cubeMeshP )
	: cubeMesh { cubeMeshP }
{
	setPosition( xP, yP );
}

void CubeObject::update() {

}

void CubeObject::draw( Shader& shader ) {
	shader.setMatrix4( "mv_matrix", transform );
	cubeMesh->draw();
}

void CubeObject::setPosition( float xP, float yP, float zP ) {
	position.x = xP;
	position.y = yP;
	position.z = zP;
	transform = computeTransform();
}

void CubeObject::setScale( float _scale )
{
	scale.x = _scale;
	scale.y = _scale;
	scale.z = _scale;
	transform = computeTransform();
}

Matrix4 CubeObject::computeTransform() {
	return Matrix4::createTranslation( position ) * Matrix4::createScale( scale );
}