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

	void bind() const;
	void unbind() const;

	void addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
	void setIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

	const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const { return mVertexBuffers; }
	const Ref<IndexBuffer>& getIndexBuffer() const { return mIndexBuffer; }
public:
	static Ref<VertexArray> Create() { return CreateRef<VertexArray>(); }
private:
	uint32_t mRendererID;
	uint32_t mVertexBufferIndex = 0;
	std::vector<Ref<VertexBuffer>> mVertexBuffers;
	Ref<IndexBuffer> mIndexBuffer;
};
