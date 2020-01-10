#include "ModelPool.h"
#include "../ObjLoader.h"

ModelPool::ModelPool(const char* path, char type) {
	std::vector<float> vertices, normals, colors;
	
	LoadObj(path, &vertices, &colors, &normals, type);

	m_iNumVtx = vertices.size() / 3;
	m_pPoints = NULL; m_pNormals = NULL; m_pTex = NULL; m_pLightTex = NULL;

	m_pPoints = new vec4[m_iNumVtx];
	m_pNormals = new vec3[m_iNumVtx];
	m_pColors = new vec4[m_iNumVtx];
	m_pTex = new vec2[m_iNumVtx];
	m_pLightTex = new vec2[m_iNumVtx];
	m_pNormalTex = new vec2[m_iNumVtx];
	m_pTangentV = new vec3[m_iNumVtx];

	for (int i = 0; i < m_iNumVtx; i++)
	{
		m_pPoints[i] = vec4(vertices[i * 3], vertices[ i * 3 + 1 ], vertices[i * 3 + 2], 1.0f);
		m_pNormals[i] = vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);
		m_pTex[i] = vec2(colors[i * 3], colors[i * 3 + 1]);
		m_pLightTex[i] = vec2(colors[i * 3], colors[i * 3 + 1]);
		m_pNormalTex[i] = vec2(colors[i * 3], colors[i * 3 + 1]);
		m_pColors[i] = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	}

	for (int i = 0; i < m_iNumVtx; i += 3) { // �T�� vertex �@��
		float dU1 = m_pNormalTex[i + 1].x - m_pNormalTex[i].x;
		float dV1 = m_pNormalTex[i + 1].y - m_pNormalTex[i].y;
		float dU2 = m_pNormalTex[i + 2].x - m_pNormalTex[i].x;
		float dV2 = m_pNormalTex[i + 2].y - m_pNormalTex[i].y;
		float f = 1.0f / (dU1 * dV2 - dU2*dV1);
		vec4 E1 = m_pPoints[i + 1] - m_pPoints[i];
		vec4 E2 = m_pPoints[i + 2] - m_pPoints[i];

		vec3 tangent;
		tangent.x = f*(dV2 * E1.x + E2.x * (-dV1));
		tangent.y = f*(dV2 * E1.y + E2.y * (-dV1));
		tangent.z = f*(dV2 * E1.z + E2.z * (-dV1));

		m_pTangentV[i] += tangent;
		m_pTangentV[i + 1] += tangent;
		m_pTangentV[i + 2] = tangent;
	}
	for (int i = 0; i < m_iNumVtx; i++)m_pTangentV[i] = normalize(m_pTangentV[i]);
	
	SetShaderName("vsPerPixelLighting.glsl", "fsPerPixelLighting.glsl");

	SetMaterials(vec4(0), vec4(1.0f, 1.0, 1.0, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	SetKaKdKsShini(0.15f, 0.8f, 0.2f, 2);

	SetShader();
}

void ModelPool::Draw()
{
	DrawingSetShader();
	glDrawArrays(GL_TRIANGLES, 0, m_iNumVtx);
}

void ModelPool::DrawW()
{
	DrawingWithoutSetShader();
	glDrawArrays(GL_TRIANGLES, 0, m_iNumVtx);
}