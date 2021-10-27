#pragma once

#include <vector>

#include "Base.h"

class VertexBuffer;
class IndexBuffer;

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void Bind() const;
	static void Unbind();

	void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
	void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

	const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return mVertexBuffers; }
	const Ref<IndexBuffer>& GetIndexBuffer() const { return mIndexBuffer; }
public:
	static Ref<VertexArray> Create() { return CreateRef<VertexArray>(); }
private:
	uint32_t mRendererID;
	uint32_t mVertexBufferIndex = 0;
	std::vector<Ref<VertexBuffer>> mVertexBuffers;
	Ref<IndexBuffer> mIndexBuffer;
};
