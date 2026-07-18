#if AXE_RENDER_HAS_VK

#include "Context_Vk_Win32.h"
#include "Renderer_Vk.h"
#include "Device_Vk.h"
#include <axe_render/vertex/Vertex.h>
#include <axe_render/command/RenderCommand.h>

namespace axe {

Context_Vk_Win32::Context_Vk_Win32(RenderDevice& device, const CreateDesc& desc)
	: Base(device, desc)
{
	_dev = &renderDevice()->vkDevice();
	
	{ // create queue
		_dev->getGraphQueue(_graphQueue_vk);
	}
	
	{ // create surface
		auto hInstance = ::GetModuleHandle(nullptr);
		AXE_ASSERT(desc.window != nullptr);
		_hwnd = desc.window->hwnd();
		_surface_vk.create_Win32(*_dev, hInstance, _hwnd);
	}
	
	{ // Command pool
		commandPool.create(*_dev, _dev->graphQueueFamilyIndex());

		commandBuffers.resizeToLocalBufSize();
		for (int i = 0; i < maxFramesInFlight; i++) {
			commandBuffers[i].create(commandPool);		
		}
	}
}

void Context_Vk_Win32::_test_LoadAssets() {
	using VertexT = VertexT_Color<Color4f, 1, Vertex_Pos>;
	
	const VkFormat imageFormat { VK_FORMAT_B8G8R8A8_SRGB };
	VkFormat depthFormat{ VK_FORMAT_UNDEFINED };
	
	{ // Depth attachment
		std::vector<::VkFormat> depthFormatList {
			VK_FORMAT_D32_SFLOAT_S8_UINT, //RenderDepthType::Depth_Float32_Stencil_UInt8
			VK_FORMAT_D24_UNORM_S8_UINT   //RenderDepthType::Depth_UNorm24_Stencil_UInt8
		};
		for (VkFormat& format : depthFormatList) {
			VkFormatProperties2 formatProperties {};
			formatProperties.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
			::vkGetPhysicalDeviceFormatProperties2(*_dev, format, &formatProperties);
			if (formatProperties.formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
				depthFormat = format;
				break;
			}
		}
		assert(depthFormat != VK_FORMAT_UNDEFINED);
	
		depthImage.createDepthStencil(*_dev, _swapChainFrameBufferSize, RenderDepthType::Depth_Float32_Stencil_UInt8);
		depthImageView.create(*_dev, depthImage, depthFormat);
	}
	
	{ // Mesh data
#if 0
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		chk(tinyobj::LoadObj(&attrib, &shapes, &materials, nullptr, nullptr, "assets/suzanne.obj"));
		
		const VkDeviceSize indexCount{ shapes[0].mesh.indices.size() };
		std::vector<Vertex> vertices{};
		std::vector<uint16_t> indices{};
		// Load vertex and index data
		for (auto& index : shapes[0].mesh.indices) {
			Vertex v{
				.pos = { attrib.vertices[index.vertex_index * 3], -attrib.vertices[index.vertex_index * 3 + 1], attrib.vertices[index.vertex_index * 3 + 2] },
				.normal = { attrib.normals[index.normal_index * 3], -attrib.normals[index.normal_index * 3 + 1], attrib.normals[index.normal_index * 3 + 2] },
				.uv = { attrib.texcoords[index.texcoord_index * 2], 1.0 - attrib.texcoords[index.texcoord_index * 2 + 1] }
			};
			vertices.push_back(v);
			indices.push_back(indices.size());
		}
#else
		// Load vertex and index data
		Vec2f frameSize = Vec2f::s_cast(swapChainFrameBufferSize());
		float m_aspectRatio = frameSize.x / frameSize.y;

		VertexT triangleVertices[3];
		triangleVertices[0].pos.set( 0.00f,  0.25f * m_aspectRatio, 0.0f);	triangleVertices[0].color[0].set( 1.0f, 0.0f, 0.0f, 1.0f);
		triangleVertices[1].pos.set( 0.25f, -0.25f * m_aspectRatio, 0.0f);	triangleVertices[1].color[0].set( 0.0f, 1.0f, 0.0f, 1.0f);
		triangleVertices[2].pos.set(-0.25f, -0.25f * m_aspectRatio, 0.0f);	triangleVertices[2].color[0].set( 0.0f, 0.0f, 1.0f, 1.0f);
#endif
		
		Int vertexBufferSize = sizeof(triangleVertices);
		vBuffer.create(*_dev, GpuBufferType::Vertex, vertexBufferSize);

		AXE_VkFence			fenceOneTime;
		AXE_VkCommandBuffer cbOneTime;
		AXE_VmaBuffer		stagingBuffer;
		fenceOneTime.create(*_dev, false);
		cbOneTime.create(commandPool);
		
		cbOneTime.beginCommand();
			stagingBuffer.create(*_dev, GpuBufferType::StagingToGpu, vertexBufferSize);
			if (auto scoped = stagingBuffer.mapMemory(vertexBufferSize)) {
				auto t = ByteSpan(reinterpret_cast<const u8*>(&triangleVertices[0]), vertexBufferSize);
				scoped.copyData(t); // 	memcpy(stagingBuffer.vmaAllocationInfo().pMappedData, triangleVertices, vertexBufferSize);
			}
			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0;			// ← Offset in staging buffer
			copyRegion.dstOffset = 0;			// ← Offset in destination buffer
			copyRegion.size = vertexBufferSize; // ← Number of bytes to copy
			vkCmdCopyBuffer(cbOneTime, stagingBuffer, vBuffer, 1, &copyRegion);
		cbOneTime.endCommand();
		
		VkSubmitInfo oneTimeSI{};
		oneTimeSI.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		oneTimeSI.commandBufferCount = 1;
		oneTimeSI.pCommandBuffers = &cbOneTime.handle();
		
		Span<VkSubmitInfo> infos(&oneTimeSI, 1);
		_graphQueue_vk.submit(infos, fenceOneTime);
		fenceOneTime.wait(::eastl::nullopt);
	}
#if 0
	{ // Shader data buffers
		for (auto i = 0; i < maxFramesInFlight; i++) {
			VkBufferCreateInfo uBufferCI{};
			uBufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			uBufferCI.size = sizeof(ShaderData);
			uBufferCI.usage = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
			VmaAllocationCreateInfo uBufferAllocCI{};
			uBufferAllocCI.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
								| VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT
								| VMA_ALLOCATION_CREATE_MAPPED_BIT;
			uBufferAllocCI.usage = VMA_MEMORY_USAGE_AUTO;
			auto err = vmaCreateBuffer(_dev->vmaAllocator()
											 , &uBufferCI
											 , &uBufferAllocCI
											 , &shaderDataBuffers[i].buffer
											 , &shaderDataBuffers[i].allocation
											 , &shaderDataBuffers[i].allocationInfo);
			VkUtil::throwIfError(err);
			VkBufferDeviceAddressInfo uBufferBdaInfo{};
			uBufferBdaInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
			uBufferBdaInfo.buffer = shaderDataBuffers[i].buffer;
			shaderDataBuffers[i].deviceAddress = ::vkGetBufferDeviceAddress(*_dev, &uBufferBdaInfo);
		}
	}
#endif
	{ // Sync objects
		fences.resizeToLocalBufSize();
		imageAcquiredSemaphores.resizeToLocalBufSize();
		
		for (auto& obj		: fences) { obj.create(*_dev, true); }
		for (auto& obj : imageAcquiredSemaphores) { obj.create(*_dev); }
	}
	
#if 0 // no need atm
	{ // Texture images
		std::vector<VkDescriptorImageInfo> textureDescriptors{};
		for (auto i = 0; i < textures.size(); i++) {
			ktxTexture* ktxTexture{ nullptr };
			std::string filename = "assets/suzanne" + std::to_string(i) + ".ktx";
			ktxTexture_CreateFromNamedFile(filename.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);
			VkImageCreateInfo texImgCI{
				.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
				.imageType = VK_IMAGE_TYPE_2D,
				.format = ktxTexture_GetVkFormat(ktxTexture),
				.extent = {.width = ktxTexture->baseWidth, .height = ktxTexture->baseHeight, .depth = 1 },
				.mipLevels = ktxTexture->numLevels,
				.arrayLayers = 1,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.tiling = VK_IMAGE_TILING_OPTIMAL,
				.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
			};
				
				
			VmaAllocationCreateInfo texImageAllocCI{ .usage = VMA_MEMORY_USAGE_AUTO };
			chk(vmaCreateImage(allocator, &texImgCI, &texImageAllocCI, &textures[i].image, &textures[i].allocation, nullptr));
			VkImageViewCreateInfo texVewCI{ .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, .image = textures[i].image, .viewType = VK_IMAGE_VIEW_TYPE_2D, .format = texImgCI.format, .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = ktxTexture->numLevels, .layerCount = 1 } };
			chk(vkCreateImageView(device, &texVewCI, nullptr, &textures[i].view));
			// Upload
			VkBuffer imgSrcBuffer{};
			VmaAllocation imgSrcAllocation{};
			VkBufferCreateInfo imgSrcBufferCI{ .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, .size = (uint32_t)ktxTexture->dataSize, .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT };
			VmaAllocationCreateInfo imgSrcAllocCI{ .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT, .usage = VMA_MEMORY_USAGE_AUTO };
			VmaAllocationInfo imgSrcAllocInfo{};
			chk(vmaCreateBuffer(allocator, &imgSrcBufferCI, &imgSrcAllocCI, &imgSrcBuffer, &imgSrcAllocation, &imgSrcAllocInfo));
			memcpy(imgSrcAllocInfo.pMappedData, ktxTexture->pData, ktxTexture->dataSize);
			VkFenceCreateInfo fenceOneTimeCI{ .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
			VkFence fenceOneTime{};
			chk(vkCreateFence(device, &fenceOneTimeCI, nullptr, &fenceOneTime));
			VkCommandBuffer cbOneTime{};
			VkCommandBufferAllocateInfo cbOneTimeAI{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, .commandPool = commandPool, .commandBufferCount = 1 };
			chk(vkAllocateCommandBuffers(device, &cbOneTimeAI, &cbOneTime));
			VkCommandBufferBeginInfo cbOneTimeBI{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT };
			chk(vkBeginCommandBuffer(cbOneTime, &cbOneTimeBI));
			VkImageMemoryBarrier2 barrierTexImage{
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
				.srcStageMask = VK_PIPELINE_STAGE_2_NONE,
				.srcAccessMask = VK_ACCESS_2_NONE,
				.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
				.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
				.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				.image = textures[i].image,
				.subresourceRange = { .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = ktxTexture->numLevels, .layerCount = 1 }
			};
			VkDependencyInfo barrierTexInfo{ .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO, .imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &barrierTexImage };
			vkCmdPipelineBarrier2(cbOneTime, &barrierTexInfo);
			std::vector<VkBufferImageCopy> copyRegions{};
			for (auto j = 0; j < ktxTexture->numLevels; j++) {
				ktx_size_t mipOffset{0};
				KTX_error_code ret = ktxTexture_GetImageOffset(ktxTexture, j, 0, 0, &mipOffset);
				copyRegions.push_back({
					.bufferOffset = mipOffset,
					.imageSubresource{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = (uint32_t)j, .layerCount = 1},
					.imageExtent{.width = ktxTexture->baseWidth >> j, .height = ktxTexture->baseHeight >> j, .depth = 1 },
				});
			}
			vkCmdCopyBufferToImage(cbOneTime, imgSrcBuffer, textures[i].image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(copyRegions.size()), copyRegions.data());
			VkImageMemoryBarrier2 barrierTexRead{
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
				.srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,
				.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				.dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
				.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				.newLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
				.image = textures[i].image,
				.subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = ktxTexture->numLevels, .layerCount = 1 }
			};
			barrierTexInfo.pImageMemoryBarriers = &barrierTexRead;
			vkCmdPipelineBarrier2(cbOneTime, &barrierTexInfo);
			chk(vkEndCommandBuffer(cbOneTime));
			VkSubmitInfo oneTimeSI{ .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO, .commandBufferCount = 1, .pCommandBuffers = &cbOneTime };
			chk(vkQueueSubmit(queue, 1, &oneTimeSI, fenceOneTime));
			chk(vkWaitForFences(device, 1, &fenceOneTime, VK_TRUE, UINT64_MAX));
			vkDestroyFence(device, fenceOneTime, nullptr);
			vmaDestroyBuffer(allocator, imgSrcBuffer, imgSrcAllocation);
			// Sampler
			VkSamplerCreateInfo samplerCI{
				.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
				.magFilter = VK_FILTER_LINEAR,
				.minFilter = VK_FILTER_LINEAR,
				.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
				.anisotropyEnable = VK_TRUE,
				.maxAnisotropy = 8.0f,
				.maxLod = (float)ktxTexture->numLevels,
			};
			chk(vkCreateSampler(device, &samplerCI, nullptr, &textures[i].sampler));
			ktxTexture_Destroy(ktxTexture);
			textureDescriptors.push_back({ .sampler = textures[i].sampler, .imageView = textures[i].view, .imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL });
		}
	}

	{ // Descriptor (indexing)
		VkResult err;
		VkDescriptorBindingFlags descVariableFlag{ VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT };
		VkDescriptorSetLayoutBindingFlagsCreateInfo descBindingFlags{};
		descBindingFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
		descBindingFlags.bindingCount = 1;
		descBindingFlags.pBindingFlags = &descVariableFlag;
		VkDescriptorSetLayoutBinding descLayoutBindingTex{};
		descLayoutBindingTex.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descLayoutBindingTex.descriptorCount = static_cast<uint32_t>(textures.size());
		descLayoutBindingTex.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		VkDescriptorSetLayoutCreateInfo descLayoutTexCI{};
		descLayoutTexCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descLayoutTexCI.pNext = &descBindingFlags;
		descLayoutTexCI.bindingCount = 1;
		descLayoutTexCI.pBindings = &descLayoutBindingTex;
		
		err = vkCreateDescriptorSetLayout(*_dev, &descLayoutTexCI, nullptr, &descriptorSetLayoutTex);
		VkUtil::throwIfError(err);
		
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize.descriptorCount = static_cast<uint32_t>(textures.size());
		VkDescriptorPoolCreateInfo descPoolCI{};
		descPoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descPoolCI.maxSets = 1;
		descPoolCI.poolSizeCount = 1;
		descPoolCI.pPoolSizes = &poolSize;
		err = vkCreateDescriptorPool(*_dev, &descPoolCI, nullptr, &descriptorPool);
		VkUtil::throwIfError(err);
		
		uint32_t variableDescCount{ static_cast<uint32_t>(textures.size()) };
		VkDescriptorSetVariableDescriptorCountAllocateInfo variableDescCountAI{};
		variableDescCountAI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT;
		variableDescCountAI.descriptorSetCount = 1;
		variableDescCountAI.pDescriptorCounts = &variableDescCount;
		VkDescriptorSetAllocateInfo texDescSetAlloc{};
		texDescSetAlloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		texDescSetAlloc.pNext = &variableDescCountAI;
		texDescSetAlloc.descriptorPool = descriptorPool;
		texDescSetAlloc.descriptorSetCount = 1;
		texDescSetAlloc.pSetLayouts = &descriptorSetLayoutTex;
		
		err = vkAllocateDescriptorSets(*_dev, &texDescSetAlloc, &descriptorSetTex);
		VkUtil::throwIfError(err);
		
		VkWriteDescriptorSet writeDescSet{};
		writeDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescSet.dstSet = descriptorSetTex;
		writeDescSet.dstBinding = 0;
		writeDescSet.descriptorCount = static_cast<uint32_t>(textureDescriptors.size());
		writeDescSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescSet.pImageInfo = textureDescriptors.data();
		vkUpdateDescriptorSets(*_dev, 1, &writeDescSet, 0, nullptr);
	}
#endif
	
	::VkShaderModule shaderModule {};
	{ // Initialize Slang shader compiler
		auto* proj = ProjectSettings::instance();
		Vector<u8> outVSBytecode;
		
		auto spvFilename = Fmt("{}/Assets/Shaders/Demo/hello_triangle.shader/Vk/pass0/hello_triangle.slang.bin", proj->importedPath());
		File::readFile(spvFilename, outVSBytecode);
		
		//Span<const u32> tmp = spanCast<const u32>(outVSBytecode.span());
		
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType	= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = axe_safe_cast_from(outVSBytecode.size()); // tmp.size_bytes();
		createInfo.pCode	= reinterpret_cast<const uint32_t*>(outVSBytecode.data()); // tmp.data();
		
		auto err = vkCreateShaderModule(*_dev, &createInfo, nullptr, &shaderModule);
		VkUtil::throwIfError(err);
	}
	
	{ // Pipeline
		VkResult err;
		VkGraphicsPipelineCreateInfo pipelineCI = {};
		pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		// Pipeline Layout ( uniform buffers, textures, etc)
		#if 0
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.size = sizeof(VkDeviceAddress);
		VkPipelineLayoutCreateInfo pipelineLayoutCI{};
		pipelineLayoutCI.sType						= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCI.setLayoutCount 			= 1;
		pipelineLayoutCI.pSetLayouts				= &descriptorSetLayoutTex;
		pipelineLayoutCI.pushConstantRangeCount 	= 1;
		pipelineLayoutCI.pPushConstantRanges		= &pushConstantRange;
		err = ::vkCreatePipelineLayout(*_dev, &pipelineLayoutCI, nullptr, &pipelineLayout);
		VkUtil::throwIfError(err);
		#else// simple triangle no need atm
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;        // No descriptor sets
		pipelineLayoutInfo.pSetLayouts = nullptr;     // No descriptor sets
		pipelineLayoutInfo.pushConstantRangeCount = 0; // No push constants
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		err = ::vkCreatePipelineLayout(*_dev, &pipelineLayoutInfo, nullptr, &pipelineLayout);
		VkUtil::throwIfError(err);
		#endif
		pipelineCI.layout = pipelineLayout;
		// vkDestroyPipelineLayout(device, pipelineLayout, nullptr); TODO: when destroy
// Dynamic State
		std::vector<VkDynamicState> dynamicStates{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
		};
		VkPipelineDynamicStateCreateInfo dynamicState {};
		dynamicState.sType				= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount	= static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates		= dynamicStates.data();
		pipelineCI.pDynamicState = &dynamicState;
// Viewport State
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType				= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount 	= 1;
		viewportState.scissorCount  	= 1;
		// pViewports and pScissors can be nullptr because the state is dynamic
		pipelineCI.pViewportState = &viewportState;		
// Shader Stage EntryFunc Setup	
		VkPipelineShaderStageCreateInfo vertStage{};
		vertStage.sType  				= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertStage.stage  				= VK_SHADER_STAGE_VERTEX_BIT;
		vertStage.module 				= shaderModule;
		vertStage.pName  				= "vs_main";

		VkPipelineShaderStageCreateInfo fragStage{};
		fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragStage.module = shaderModule;
		fragStage.pName = "ps_main";
		
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages {
			vertStage,
			fragStage,
		};
		
		pipelineCI.stageCount	= VkUtil::castUInt32(shaderStages.size());
		pipelineCI.pStages		= shaderStages.data();
// Vertex Input State
		VkVertexInputBindingDescription vertexBinding{};
		vertexBinding.binding	= 0;
		vertexBinding.stride	= sizeof(VertexT);
		vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		VkVertexInputAttributeDescription VS_POSITION = {};
		VS_POSITION.location = 0;
		VS_POSITION.binding  = 0;
		VS_POSITION.format   = VK_FORMAT_R32G32B32_SFLOAT;
		VS_POSITION.offset   = offsetof(VertexT, pos); // memberOffset(&VertexT::pos);
		VkVertexInputAttributeDescription VS_COLOR = {};
		VS_COLOR.location 	= 1;
		VS_COLOR.binding  	= 0;
		VS_COLOR.format   	= VK_FORMAT_R32G32B32A32_SFLOAT;
		VS_COLOR.offset   	= offsetof(VertexT, color); // memberOffset(&VertexT::color);
		std::vector<VkVertexInputAttributeDescription> vertexAttributes{
			VS_POSITION,
			VS_COLOR,
		};
		VkPipelineVertexInputStateCreateInfo vertexInputState{};
		vertexInputState.sType								= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputState.vertexBindingDescriptionCount		= 1;
		vertexInputState.pVertexBindingDescriptions			= &vertexBinding;
		vertexInputState.vertexAttributeDescriptionCount	= static_cast<uint32_t>(vertexAttributes.size());
		vertexInputState.pVertexAttributeDescriptions		= vertexAttributes.data();
		pipelineCI.pVertexInputState = &vertexInputState;
// Input Assembly State
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState {};
		inputAssemblyState.sType 								= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyState.pNext 								= nullptr;
		inputAssemblyState.flags 								= 0;
		inputAssemblyState.topology								= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyState.primitiveRestartEnable				= VK_FALSE;
		pipelineCI.pInputAssemblyState = &inputAssemblyState;		
// Rasterization State
		VkPipelineRasterizationStateCreateInfo rasterizationState{};
		rasterizationState.sType 								= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationState.pNext 								= nullptr;
		rasterizationState.flags 								= 0;
		rasterizationState.depthClampEnable						= VK_FALSE;
		rasterizationState.rasterizerDiscardEnable				= VK_FALSE;
		rasterizationState.polygonMode							= VK_POLYGON_MODE_FILL;
		rasterizationState.cullMode								= VK_CULL_MODE_FRONT_BIT; // VK_CULL_MODE_NONE;
		rasterizationState.frontFace							= VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationState.depthBiasEnable						= VK_FALSE;
		//float depthBiasConstantFactor;
		//float depthBiasClamp;
		//float depthBiasSlopeFactor;
		rasterizationState.lineWidth							= 1.0f;
		pipelineCI.pRasterizationState = &rasterizationState;
// Multisample State: Configures anti-aliasing
		VkPipelineMultisampleStateCreateInfo multisampleState{};
		multisampleState.sType				  = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleState.sampleShadingEnable  = VK_FALSE;
		pipelineCI.pMultisampleState = &multisampleState;
// Depth and Stencil State
		VkPipelineDepthStencilStateCreateInfo depthStencilState{};
		depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilState.depthTestEnable = VK_TRUE;
		depthStencilState.depthWriteEnable = VK_TRUE;
		depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		AXE_UNUSED(depthStencilState);
		pipelineCI.pDepthStencilState = nullptr; // &depthStencilState; simple triangle no need depth
// Color Blend State
		VkPipelineColorBlendAttachmentState blendAttachment{};
		blendAttachment.colorWriteMask = 0xF; // VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		blendAttachment.blendEnable = VK_FALSE; // simple triangle could disable blending.
		
		VkPipelineColorBlendStateCreateInfo colorBlendState{};
		colorBlendState.sType 								= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendState.pNext 								= nullptr;
		colorBlendState.flags 								= 0;
		colorBlendState.logicOpEnable						= VK_FALSE;
		colorBlendState.logicOp								= VK_LOGIC_OP_COPY;
		colorBlendState.attachmentCount 					= 1;
		colorBlendState.pAttachments						= &blendAttachment;
		//float blendConstants[4];
		pipelineCI.pColorBlendState = &colorBlendState;
// -----------
		VkPipelineRenderingCreateInfo renderingCI {};
		renderingCI.sType				 		= VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		renderingCI.colorAttachmentCount 		= 1;
		renderingCI.pColorAttachmentFormats 	= &imageFormat;
		renderingCI.depthAttachmentFormat   	= depthFormat;
		if (pipelineCI.pDepthStencilState == nullptr) {
			renderingCI.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
		}
		pipelineCI.pNext = &renderingCI;
		
		err = ::vkCreateGraphicsPipelines(*_dev
										, VK_NULL_HANDLE
										, 1
										, &pipelineCI
										, nullptr
										, &pipeline);
		VkUtil::throwIfError(err);
	}
}

void Context_Vk_Win32::_test_WaitForPreviousFrame() {
	auto& fence = fences[frameIndex];
	auto err = vkWaitForFences(*_dev, 1, &fence.handle(), true, UINT64_MAX);
	VkUtil::throwIfError(err);
	
	err = vkResetFences(*_dev, 1, &fence.handle());
	VkUtil::throwIfError(err);
}

void Context_Vk_Win32::onBeginRender() {
	AXE_RUN_ONCE(_test_LoadAssets());
	
	// Skip rendering when window is minimized (extent 0x0)
	if (_swapChainFrameBufferSize.x <= 0 || _swapChainFrameBufferSize.y <= 0) {
		return;
	}
	
	if (updateSwapchain) {
		_recreateSwapChain();
		updateSwapchain = false;
	}
	
	_test_WaitForPreviousFrame();
	
	VkResult err = _swapChain_vk.acquireNextImage(swapChainImageIndex
								 , imageAcquiredSemaphores[frameIndex]
								 , VK_NULL_HANDLE
								 , UINT64_MAX);

	if (err != VK_SUCCESS && err != VK_SUBOPTIMAL_KHR) {
		VkUtil::throwIfError(err);
	}

	_onBeginFrame();
	_onRender_RenderGraph();
}

void Context_Vk_Win32::_onBeginFrame() {
	// Build command buffer
	auto& cb = commandBuffers[frameIndex];
	cb.resetAndReleaseResource();
	cb.beginCommand();
	
	VkRenderingAttachmentInfo colorAttachmentInfo{};
	colorAttachmentInfo.sType					= VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	colorAttachmentInfo.imageView				= swapchainImageViews[swapChainImageIndex]->handle();
	colorAttachmentInfo.imageLayout 			= VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
	colorAttachmentInfo.loadOp					= VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachmentInfo.storeOp 				= VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentInfo.clearValue.color	 = {{0.0f, 0.2f, 0.4f, 1.0f} /*{0.0f, 0.0f, 0.0f, 1.0f}*/}; // clear color

#if 0
	VkRenderingAttachmentInfo depthAttachmentInfo{};
	depthAttachmentInfo.sType					= VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	depthAttachmentInfo.imageView				= depthImageView;
	depthAttachmentInfo.imageLayout 			= VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
	depthAttachmentInfo.loadOp					= VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachmentInfo.storeOp					= VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachmentInfo.clearValue				= {};
	depthAttachmentInfo.clearValue.depthStencil = {1.0f,  0};
#endif
	
	{ // Transition swapchain image from UNDEFINED/PRESENT_SRC_KHR to ATTACHMENT_OPTIMAL for dynamic rendering
		VkImageMemoryBarrier2 renderBeginBarrier{};
		renderBeginBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		renderBeginBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		renderBeginBarrier.newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
		renderBeginBarrier.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
		renderBeginBarrier.srcAccessMask = 0;
		renderBeginBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		renderBeginBarrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
		renderBeginBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		renderBeginBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		renderBeginBarrier.image = swapchainImages[swapChainImageIndex];
		renderBeginBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		renderBeginBarrier.subresourceRange.levelCount = 1;
		renderBeginBarrier.subresourceRange.layerCount = 1;

		VkDependencyInfo renderBeginDependencyInfo{};
		renderBeginDependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		renderBeginDependencyInfo.imageMemoryBarrierCount = 1;
		renderBeginDependencyInfo.pImageMemoryBarriers = &renderBeginBarrier;

		vkCmdPipelineBarrier2(cb, &renderBeginDependencyInfo);
	}

	VkRenderingInfo renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	Rect2i tmpRenderArea(0, 0, _swapChainFrameBufferSize.x, _swapChainFrameBufferSize.y);
	renderingInfo.renderArea = VkUtil::castVkRect2D(tmpRenderArea);
	renderingInfo.layerCount = 1;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachmentInfo;
	renderingInfo.pDepthAttachment = nullptr; // &depthAttachmentInfo;
	
	vkCmdBeginRendering(cb, &renderingInfo);
}

void Context_Vk_Win32::_onRender_RenderGraph() {
	_onUpdate_RenderGraph();
}

void Context_Vk_Win32::_onUpdate_RenderGraph() { // like update uniform buffer
#if 0 // Update shader data
	shaderData.projection = glm::perspective(glm::radians(45.0f), (float)windowSize.x / (float)windowSize.y, 0.1f, 32.0f);
	shaderData.view = glm::translate(glm::mat4(1.0f), camPos);
	for (auto i = 0; i < 3; i++) {
		auto instancePos = glm::vec3((float)(i - 1) * 3.0f, 0.0f, 0.0f);
		shaderData.model[i] = glm::translate(glm::mat4(1.0f), instancePos) * glm::mat4_cast(glm::quat(objectRotations[i]));
	}
	memcpy(shaderDataBuffers[frameIndex].allocationInfo.pMappedData, &shaderData, sizeof(ShaderData));
#endif
	
#if 0
	std::array<VkImageMemoryBarrier2, 2> outputBarriers{
		VkImageMemoryBarrier2{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = 0,
			.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
			.image = swapchainImages[imageIndex],
			.subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1 }
		},
		VkImageMemoryBarrier2{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
			.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
			.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
			.image = depthImage,
			.subresourceRange{.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, .levelCount = 1, .layerCount = 1 }
		}
	};

	VkDependencyInfo barrierDependencyInfo{ .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO, .imageMemoryBarrierCount = 2, .pImageMemoryBarriers = outputBarriers.data() };
	vkCmdPipelineBarrier2(cb, &barrierDependencyInfo);
#endif
}

void Context_Vk_Win32::onCommit(RenderCommandBuffer& cmdBuf) {
	// Skip rendering when window is minimized (extent 0x0)  simple for now
	if (_swapChainFrameBufferSize.x <= 0 || _swapChainFrameBufferSize.y <= 0) {
		return;
	}
	
	_dispatch(this, cmdBuf);
	_onEndFrame();
}

void Context_Vk_Win32::_onEndFrame() {
	auto& cb = commandBuffers[frameIndex];

	vkCmdEndRendering(cb);
#if 0 // pipelineBarrier
	VkImageMemoryBarrier2 barrierPresent{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
		.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstAccessMask = 0,
		.oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		.image = swapchainImages[imageIndex],
		.subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .levelCount = 1, .layerCount = 1 }
	};
	VkDependencyInfo barrierPresentDependencyInfo{ .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO, .imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &barrierPresent };
#else
	VkImageMemoryBarrier2 barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrier.pNext = nullptr;
	barrier.oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	barrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	barrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
	barrier.dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
	barrier.dstAccessMask = 0;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = swapchainImages[swapChainImageIndex];  // Your current swapchain image
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	// Create the dependency info that wraps the barrier
	VkDependencyInfo barrierPresentDependencyInfo{};
	barrierPresentDependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	barrierPresentDependencyInfo.pNext = nullptr;
	barrierPresentDependencyInfo.dependencyFlags = 0;
	barrierPresentDependencyInfo.memoryBarrierCount = 0;
	barrierPresentDependencyInfo.pMemoryBarriers = nullptr;
	barrierPresentDependencyInfo.bufferMemoryBarrierCount = 0;
	barrierPresentDependencyInfo.pBufferMemoryBarriers = nullptr;
	barrierPresentDependencyInfo.imageMemoryBarrierCount = 1;
	barrierPresentDependencyInfo.pImageMemoryBarriers = &barrier;
#endif
	vkCmdPipelineBarrier2(cb, &barrierPresentDependencyInfo);
	cb.endCommand();
}


#if 1
void Context_Vk_Win32::onCmd_SetViewport(RenderCommand_SetViewport& cmd) {
	auto& cb = commandBuffers[frameIndex];

	::VkViewport vp {};

	const auto& rect = cmd.rect;
#if 1
	// flip Y - in vulkan Y is downward, while DX and OpenGL is up
	// VkViewport::height (VK_KHR_Maintenance1) can be negative
	vp.x        = rect.x;
	vp.y        = rect.h - rect.y;
	vp.width    = rect.w;
	vp.height   = -rect.h; // flip Y
#else
	vp.x        = rect.x;
	vp.y        = rect.y;
	vp.width    = rect.w;
	vp.height   = rect.h;
#endif
	vp.minDepth = cmd.minDepth;
	vp.maxDepth = cmd.maxDepth;
	
	::vkCmdSetViewport(cb, 0, 1, &vp);
}

void Context_Vk_Win32::onCmd_SetScissorRect(RenderCommand_SetScissorRect& cmd) {
	auto& cb = commandBuffers[frameIndex];
	auto& rect = cmd.rect;

	VkRect2D scissor {};
	scissor.extent= {};
	scissor.extent.width  = static_cast<uint32_t>(rect.size.x);
	scissor.extent.height = static_cast<uint32_t>(rect.size.y);
	::vkCmdSetScissor(cb, 0, 1, &scissor);
}
#endif

void Context_Vk_Win32::onCmd_ClearFrameBuffers(RenderCommand_ClearFrameBuffers& cmd) {
	AXE_TODO("may Remove later");
	
	auto& cb = commandBuffers[frameIndex];
	
	Rect2i tmpClearRect(0, 0, _swapChainFrameBufferSize.x, _swapChainFrameBufferSize.y);
	
	// clear back buffer(color buffer)
	if (cmd.color.has_value()) {
		auto& clearColor = cmd.color.value();
		
		VkClearAttachment clearAtt = {};
		clearAtt.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		clearAtt.colorAttachment = 0;
		clearAtt.clearValue.color = {{clearColor.r, clearColor.g, clearColor.b, clearColor.a}};
		
		VkClearRect clearRect = {};
		clearRect.rect = VkUtil::castVkRect2D(tmpClearRect);
		clearRect.baseArrayLayer = 0;
		clearRect.layerCount = 1;

		vkCmdClearAttachments(cb, 1, &clearAtt, 1, &clearRect);
	}

#if 0 // TODO
	// clear depth&stencil buffer
	if (_depthStencilView && (cmd.depth.has_value() || cmd.stencil.has_value())) {
		ctx->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, *cmd.depth, static_cast<UINT8>(*cmd.stencil));
	}
#endif
}

void Context_Vk_Win32::onCmd_DrawCall(RenderCommand_DrawCall& cmd) {
	auto& cb = commandBuffers[frameIndex];

	vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	
	//	vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSetTex, 0, nullptr);
	VkDeviceSize vOffset{ 0 };
	vkCmdBindVertexBuffers(cb, 0, 1, &vBuffer.handle(), &vOffset);
	
#if 0
	AXE_TODO("draw by index");
	vkCmdBindIndexBuffer(cb, vBuffer, vBufSize, VK_INDEX_TYPE_UINT16);
	vkCmdPushConstants(cb, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VkDeviceAddress), &shaderDataBuffers[frameIndex].deviceAddress);
	vkCmdDrawIndexed(cb, indexCount, 3, 0, 0, 0);
#else
	::vkCmdDraw(cb, 3, 1, 0, 0);
#endif
}


void Context_Vk_Win32::onEndRender() {
	// Skip rendering when window is minimized (extent 0x0)  simple for now 
	if (_swapChainFrameBufferSize.x <= 0 || _swapChainFrameBufferSize.y <= 0) {
		return;
	}
	
	auto& cb = commandBuffers[frameIndex];

	// Submit to graphics queue
	VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submitInfo{};
	submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount	= 1;
	submitInfo.pWaitSemaphores		= &imageAcquiredSemaphores[frameIndex].handle();
	submitInfo.pWaitDstStageMask	= &waitStages;
	submitInfo.commandBufferCount	= 1;
	submitInfo.pCommandBuffers		= &cb.handle();
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores	= &renderCompleteSemaphores[swapChainImageIndex].handle();
	
	Span<VkSubmitInfo> sInfos(&submitInfo, 1);
	_graphQueue_vk.submit(sInfos, fences[frameIndex]);
	
	VkPresentInfoKHR presentInfo {};
	presentInfo.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount	= 1;
	presentInfo.pWaitSemaphores 	= &renderCompleteSemaphores[swapChainImageIndex].handle();
	presentInfo.swapchainCount		= 1;
	presentInfo.pSwapchains			= &_swapChain_vk.handle();
	presentInfo.pImageIndices		= &swapChainImageIndex;
	
	auto err = vkQueuePresentKHR(_graphQueue_vk, &presentInfo);
	if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
		updateSwapchain = true;
	} else {
		VkUtil::throwIfError(err);
	}
	
	frameIndex = (frameIndex + 1) % maxFramesInFlight;
}

void Context_Vk_Win32::onSetSwapChainFrameBufferSize(const Vec2i& newSize) {
	Base::onSetSwapChainFrameBufferSize(newSize);
	
	updateSwapchain = true;
}

void Context_Vk_Win32::_recreateSwapChain() {
	AXE_ASSERT(_surface_vk.handle() != VK_NULL_HANDLE);
	_dev->waitIdle();
	
	VkSurfaceFormatKHR surfaceFormat;
	surfaceFormat.format		= VK_FORMAT_B8G8R8A8_SRGB; // VkUtil::getVkColorType(_swapChainDesc.colorDesc.colorType);
	surfaceFormat.colorSpace	= VK_COLORSPACE_SRGB_NONLINEAR_KHR; // VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

	VkPresentModeKHR presentMode = _device->vsync() 
		? VK_PRESENT_MODE_FIFO_KHR
		: VK_PRESENT_MODE_IMMEDIATE_KHR;

	_swapChain_vk.create(*_dev
						, _surface_vk
						, surfaceFormat
						, maxFramesInFlight/*_swapChainDesc.backBufferCount*/
						, presentMode);

	auto cap = _surface_vk.getCapabilities();
	_createBackBuffers(VkUtil::castVec2i(cap.currentExtent));
}

void Context_Vk_Win32::_createBackBuffers(const Vec2i& frameSize) {
	AXE_ASSERT(_swapChain_vk.handle() != VK_NULL_HANDLE);
	_swapChainFrameBufferSize = Math::max(/*RenderSwapChain::kMinFrameSize*/ Vec2i(8,8), frameSize);
	
	_swapChain_vk.getImages(swapchainImages);

	const VkFormat imageFormat{ VK_FORMAT_B8G8R8A8_SRGB }; // same as surfaceFormat.format
	
	Int imageCount = swapchainImages.size();
	swapchainImageViews.resize(imageCount); // _backBuffers_vk.resize(imageCount);
	for (Int i = 0; i < imageCount; ++i) {
		auto& dst = swapchainImageViews[i];
		if (!dst)
			dst.reset(new AXE_VkImageView());
		dst->create(*_dev, swapchainImages[i], imageFormat);
	}
	
	// One render-complete semaphore per swapchain image so a semaphore is never reused
	// while the presentation engine still waits on it.
	Int prevRenderCompleteCount = renderCompleteSemaphores.size();
	renderCompleteSemaphores.resize(imageCount);
	for (Int i = prevRenderCompleteCount; i < imageCount; ++i) {
		renderCompleteSemaphores[i].create(*_dev);
	}
}

} // namespace axe

#endif // AXE_RENDER_HAS_VK