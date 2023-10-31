#ifndef CUBEOBJECT_H
#define CUBEOBJECT_H

#include "../engine/maths/Matrix4.h"

class Shader;
class CubeMesh;

class CubeObject {
public:
	CubeObject( float xP, float yP, CubeMesh* cubeMeshP );

	void update();
	void draw( Shader& shader );

	float getX() const { return position.x; }
	float getY() const { return position.y; }
	
	void setPosition( float xP, float yP, float zP = -4.0f );
	void setScale( float scale );

	Matrix4 computeTransform();

private:
	Vector3 position { 0.0f, 0.0f, -4.0f };
	Vector3 scale { 1.0f, 1.0f, 1.0f };
	Matrix4 transform {};

	CubeMesh* cubeMesh { nullptr };
};

#endif