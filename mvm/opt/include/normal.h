#pragma once

#include <triangle_mesh.h>

namespace normal {
	core::tmesh_cptr normal_estimation(
		const core::tmesh_cptr& mesh,
		bool area_weighted = false
	);
}

// How to calculate normals for a trianglemesh ? -For Beginners - GameDev.net
// https://www.gamedev.net/forums/topic/655595-how-to-calculate-normals-for-a-trianglemesh/
// c++ - Calculating normals in a triangle mesh - Stack Overflow
// https://stackoverflow.com/questions/6656358/calculating-normals-in-a-triangle-mesh 
// algorithm - General method for calculating Smooth vertex normals with 100 % smoothness - Stack Overflow
// https://stackoverflow.com/questions/45477806/general-method-for-calculating-smooth-vertex-normals-with-100-smoothness 
// How does Blender calculate vertex normals ? -Stack Overflow 
// https://stackoverflow.com/questions/25100120/how-does-blender-calculate-vertex-normals 
// Weighted Vertex Normals
// http://www.bytehazard.com/articles/vertnorm.html 
// c#  - How to calculate verticle normals of triangular mesh - Game Development Stack Exchange
// https://gamedev.stackexchange.com/questions/137394/how-to-calculate-verticle-normals-of-triangular-mesh 
// Any reliable polygon normal calculation code ? -Game Development Stack Exchange
// https://gamedev.stackexchange.com/questions/8191/any-reliable-polygon-normal-calculation-code 
