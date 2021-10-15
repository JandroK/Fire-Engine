#include "Mesh.h"

#include "Glew/include/glew.h"

Mesh::Mesh()
{
}


void Mesh::SetupMesh()
{	
	//0
	vertex.push_back(-1.f);
	vertex.push_back(1.f);
	vertex.push_back(0.f);
	//1
	vertex.push_back(1.f);
	vertex.push_back(1.f);
	vertex.push_back(0.f);
	//2
	vertex.push_back(-1.f);
	vertex.push_back(-1.f);
	vertex.push_back(0.f);
	//3
	vertex.push_back(1.f);
	vertex.push_back(-1.f);
	vertex.push_back(0.f);

	//orden de lectura para los  vertex
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(1);
	indices.push_back(1);
	indices.push_back(0);
	indices.push_back(2);

	// Vertex Buffer GL_ARRAY_BUFFER
	glGenBuffers(1, (uint*)&(vertexBufferId));
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex.size() * 3, &vertex[0], GL_STATIC_DRAW);

	//Index Buffer GL_ELEMENT_ARRAY_BUFFER
	glGenBuffers(1, (uint*)&(indexBufferId));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * numIndices, &indices[0], GL_STATIC_DRAW);
}

void Mesh::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//-- Buffers--//
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	/*glBindBuffer(GL_ARRAY_BUFFER, textureBufferId);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindTexture(GL_TEXTURE_2D, textureId);
	*/
	//-- Draw --//
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);

	//-- UnBind Buffers--//
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/*glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	glBindTexture(GL_TEXTURE_2D, 0);*/

	//--Disables States--//
	glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}