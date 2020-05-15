#ifndef MESH_3D_H
#define MESH_3D_H

#include "TriangleAttribute.h"
#include "VertexAttribute.h"
#include "Logger.h"
#include <Eigen/Dense>
#include <map>
#include <memory>
#include <utility>
#include "Defs.h"
#include <string>
#include <vector>

using Vertex = uint32_t*;

class Mesh3D
{
public:
    enum class EVertexAttribute
    {
        Normal,
        Color,
        UVCoordinates
    };

    enum class ETriangleAttribute
    {
        Normal,
        Color
    };

private:
    class HalfEdgeDS
    {
    private:
        struct HalfEdge;

        struct Vertex
        {
            Vertex(uint32_t vertexID, const Eigen::Vector3f& pos)
                : id{vertexID}
                , position{pos} {};

            uint32_t				id;
            const Eigen::Vector3f&  position;
            std::weak_ptr<HalfEdge> wspOutgoingHalfEdge;
        };

        struct Face
        {
            Face(uint32_t faceID)
                : id{faceID} {};
            uint32_t id;
            std::weak_ptr<HalfEdge> wspBoundingHalfEdge;
        };

        struct HalfEdge
        {
            std::weak_ptr<HalfEdge> wspOppositeHalfeEdge;
            std::weak_ptr<HalfEdge> wspNextHalfeEdge;
            std::weak_ptr<HalfEdge> wspPreviousHalfeEdge;

            std::shared_ptr<Vertex> spDestinationVertex;
            std::shared_ptr<Face>   spFace;
        };

    public:
        HalfEdgeDS(Mesh3D& mesh)
            : m_mesh(mesh){};

        void InitHalfEdgeDS(void);

    private:
    	friend class Mesh3D;
        friend class oneRingFaceIterator;
        void createFace(const std::pair<int, int>& e0,
                        const std::pair<int, int>& e1,
                        const std::pair<int, int>& e2,
                        std::shared_ptr<Vertex>    vertex0,
                        std::shared_ptr<Vertex>    vertex1,
                        std::shared_ptr<Vertex>    vertex2,
                        uint32_t                   faceIdx);

    private:
        Mesh3D&                                                  m_mesh;
        std::map<std::pair<int, int>, std::shared_ptr<HalfEdge>> edges;
        std::vector<std::shared_ptr<Vertex>>                     vertices;
        std::vector<std::shared_ptr<Face>>                       faces;
    }; // class HalfEdgeDS END

public:
    explicit Mesh3D(const std::vector<Eigen::Vector3f>& vertexMatrix,
                    const std::vector<uint32_t>&        indexVector,
                    const std::string& name);

    bool HasVertices(void) { return !m_vertices.empty(); };
    bool HasUVCoordinates(void) { return !m_uvCoordinates.empty(); };
    bool HasNormals(void) { return GetVertexAttribute(EVertexAttribute::Normal) != nullptr; };

    VertexAttribute& AddVertexAttribute(EVertexAttribute vertexAttribute);

    VertexAttribute*   GetVertexAttribute(EVertexAttribute vertexAttribute);
    TriangleAttribute* GetTriangleAttribute(ETriangleAttribute triangleAttribute);
    void IterateAllFaces() const;


private:
	friend class OpenGL3DDataObject;
	friend class vertexIterator;
	friend class faceIterator;
	friend class oneRingFaceIterator;
	std::string m_name;
    std::vector<uint32_t>        m_indices;
    std::vector<Eigen::Vector3f> m_vertices;
    std::vector<Eigen::Vector2f> m_uvCoordinates;

    std::vector<std::unique_ptr<VertexAttribute>>   m_vertexAttributes;
    std::vector<std::unique_ptr<TriangleAttribute>> m_TriangleAttributes;
    HalfEdgeDS m_halfEdgeDS;
};


class vertexIterator
{
public:
	explicit vertexIterator(Mesh3D& rMesh) : m_rMesh(rMesh){}
		
	uint32_t operator*() {return m_rMesh.m_indices[idx];}
	vertexIterator& operator++() { ++idx; return *this; }
	bool operator != (const vertexIterator& other) {  return idx != other.idx; }

	vertexIterator begin() {return vertexIterator(0, m_rMesh);};
	vertexIterator end() {return vertexIterator(endIdx, m_rMesh);};
private:
	explicit vertexIterator(uint32_t i, Mesh3D& rMesh) :  m_rMesh(rMesh), idx{i} {};

private:
	Mesh3D& m_rMesh;
	uint32_t idx = 0;
	uint32_t endIdx = m_rMesh.m_indices.size();
};

class faceIterator
{
public:
	explicit faceIterator(Mesh3D& rMesh) : m_rMesh(rMesh){}
		
	uint32_t operator*() {return m_rMesh.m_indices[idx];}
	faceIterator& operator++() { idx+=3; return *this; }
	bool operator != (const faceIterator& other) {  return idx != other.idx; }

	faceIterator begin() {return faceIterator(0, m_rMesh);};
	faceIterator end() {return faceIterator(endIdx, m_rMesh);};
private:
	explicit faceIterator(uint32_t i, Mesh3D& rMesh) :  m_rMesh(rMesh), idx{i} {};

private:
	Mesh3D& m_rMesh;
	uint32_t idx = 0;
	uint32_t endIdx = m_rMesh.m_indices.size();
};

class oneRingFaceIterator
{
public:
	explicit oneRingFaceIterator(Mesh3D& rMesh, uint32_t vertexID) : m_rMesh(rMesh), m_startVertexID(vertexID)
	{
		ASSERT(vertexID >= 0 || vertexID < rMesh.m_vertices.size()) 
		m_wspHalfEdge = m_rMesh.m_halfEdgeDS.vertices[vertexID]->wspOutgoingHalfEdge;
		m_currentFaceID = m_wspHalfEdge.lock()->spFace->id;
		m_startFaceID = m_currentFaceID;
	}
		
	uint32_t operator*() {return m_currentFaceID;}
	oneRingFaceIterator& operator++() 
	{ 
		m_wspHalfEdge = m_wspHalfEdge.lock()->wspNextHalfeEdge.lock()->wspNextHalfeEdge;
		m_wspHalfEdge = m_wspHalfEdge.lock()->wspOppositeHalfeEdge; 
		m_currentFaceID = m_wspHalfEdge.lock()->spFace->id;
		return *this; 
	}
	bool operator != (const oneRingFaceIterator& other) 
	{  
		
		if(m_bStopIterating)	
		{
			m_bStopIterating = false;
			return false;
		}

		auto wspTmpHalfEdge = m_wspHalfEdge.lock()->wspNextHalfeEdge.lock()->wspNextHalfeEdge;
		wspTmpHalfEdge = wspTmpHalfEdge.lock()->wspOppositeHalfeEdge; 
		if(wspTmpHalfEdge.lock()->spFace->id == other.m_startFaceID)
		{
			m_bStopIterating = true;
		}
		return true;
	}

	oneRingFaceIterator begin() {return oneRingFaceIterator(m_rMesh, m_startVertexID);};
	oneRingFaceIterator end() {return oneRingFaceIterator(m_rMesh, m_startVertexID);};

private:

	Mesh3D& m_rMesh;
	std::weak_ptr<Mesh3D::HalfEdgeDS::HalfEdge> m_wspHalfEdge;
	uint32_t m_startVertexID = 0;
	uint32_t m_startFaceID = 0;
	uint32_t m_currentFaceID = 0;
	bool m_bStopIterating = false;
};
#endif
