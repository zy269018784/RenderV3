#include <Mesh/Mesh.h>
#
namespace Render {
	Mesh::Mesh(Transform worldFromObject, Transform objectFromWorld, Material material) :
		worldFromObject(worldFromObject), objectFromWorld(objectFromWorld), material(material){
		//cout << "Mesh::Mesh " << endl;
		//showTransform(worldFromObject);
		//
		//showTransform(objectFromWorld);
	}
}