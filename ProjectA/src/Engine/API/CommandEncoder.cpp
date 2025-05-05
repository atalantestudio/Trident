#include "pch.hpp"

#include "Engine/API/CommandEncoder.hpp"

namespace ProjectA {
	template<typename IndexType>
	void CommandEncoder::bindIndexBuffer(const ref<Buffer>& indexBuffer, uint64 offset) {
		bindIndexBuffer(indexBuffer, offset, sizeof(IndexType));
	}

	template void CommandEncoder::bindIndexBuffer<uint16>(const ref<Buffer>& indexBuffer, uint64 offset);
	template void CommandEncoder::bindIndexBuffer<uint32>(const ref<Buffer>& indexBuffer, uint64 offset);
}