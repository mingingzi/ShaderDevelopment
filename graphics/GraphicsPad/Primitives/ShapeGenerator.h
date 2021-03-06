#pragma once
#include <Primitives\ShapeData.h>
typedef unsigned int uint;

class ShapeGenerator
{
	static ShapeData makePlaneVerts(uint dimensions);
	static ShapeData makePlaneIndices(uint dimensions);
public:
	static ShapeData makeTriangle();
	static ShapeData makeCube();
	static ShapeData makePlane(uint dimensions);
	static ShapeData makeSphere(uint tesselation);
};

