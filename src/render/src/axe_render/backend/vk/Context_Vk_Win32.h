#pragma once

#if AXE_OS_WINDOWS
#if AXE_RENDER_HAS_VK

#include "Context_Vk_Base.h"

namespace axe {

class Context_Vk_Win32 : public Context_Vk_Base {
	AXE_RTTI_INFO(Context_Vk_Win32, Context_Vk_Base)
public:
	Context_Vk_Win32(RenderDevice& device, const CreateDesc& desc);

	Device_Vk*			renderDevice();
	AXE_VkSurfaceKHR&	surface() { return _surface_vk; }
	
	virtual void onBeginRender() final;
	virtual void onEndRender() final;

	virtual void onSetSwapChainFrameBufferSize(const Vec2i& newSize) final;
	virtual void onCommit(RenderCommandBuffer& cmdBuf) final;
	
	void onCmd_SetViewport			(RenderCommand_SetViewport& cmd) { AXE_TODO("onCmd_SetViewport"); }
	void onCmd_SetScissorRect		(RenderCommand_SetScissorRect& cmd) { AXE_TODO("onCmd_SetScissorRect"); }
	void onCmd_ClearFrameBuffers	(RenderCommand_ClearFrameBuffers& cmd) { AXE_TODO("onCmd_ClearFrameBuffers"); }
	void onCmd_SwapBuffers			(RenderCommand_SwapBuffers& cmd) { AXE_TODO("onCmd_SwapBuffers"); }
	void onCmd_DrawCall				(RenderCommand_DrawCall& cmd) { AXE_TODO("onCmd_DrawCall"); }
	
	constexpr static const size_t maxFramesInFlight = 2 /*RenderSwapChain::kFrameBufferWidth*/;
	
	uint32_t 									imageIndex{ 0 };
	uint32_t 									frameIndex{ 0 };

	//VkInstance								instance{ VK_NULL_HANDLE };
	//VkDevice									device{ VK_NULL_HANDLE };
	//VkQueue		 							queue{ VK_NULL_HANDLE };
	//VkSurfaceKHR	 							surface{ VK_NULL_HANDLE };
	bool updateSwapchain{ false };
	
	AXE_VkCommandPool							commandPool;
	VkPipeline									pipeline{ VK_NULL_HANDLE };
	VkPipelineLayout							pipelineLayout{ VK_NULL_HANDLE };
	
	//VkImage									depthImage;
	//VmaAllocator								allocator{ VK_NULL_HANDLE };
	//VmaAllocation								depthImageAllocation;
	AXE_VmaImage								depthImage;
	AXE_VkImageView								depthImageView;
	
	Vector<VkImage>								swapchainImages;
	Vector<UPtr<AXE_VkImageView>, 4>			swapchainImageViews; // TODO -> BackBuffer_Vk
	Vector<AXE_VkCommandBuffer, maxFramesInFlight>	commandBuffers;
	Vector<AXE_VkFence,			maxFramesInFlight>	fences;
	Vector<AXE_VkSemaphore,		maxFramesInFlight>	imageAcquiredSemaphores;
	Vector<AXE_VkSemaphore>						renderCompleteSemaphores;
	
	//VmaAllocation								vBufferAllocation{ VK_NULL_HANDLE };
	//VkBuffer									vBuffer{ VK_NULL_HANDLE };
	AXE_VmaBuffer								vBuffer;

#if 0
	struct ShaderData {
		Mat4f projection;
		Mat4f view;
		Mat4f model[3];
		Vec4f lightPos{ 0.0f, -10.0f, 10.0f, 0.0f };
		uint32_t selected{ 1 };
	} shaderData{};
	struct ShaderDataBuffer {
		VmaAllocation allocation{ VK_NULL_HANDLE };
		VmaAllocationInfo allocationInfo{};
		VkBuffer buffer{ VK_NULL_HANDLE };
		VkDeviceAddress deviceAddress{};
	};
	std::array<ShaderDataBuffer, maxFramesInFlight> shaderDataBuffers;

	struct Texture {
		VmaAllocation allocation{ VK_NULL_HANDLE };
		VkImage image{ VK_NULL_HANDLE };
		VkImageView view{ VK_NULL_HANDLE };
		VkSampler sampler{ VK_NULL_HANDLE };
	};
	std::array<Texture, 3> textures{};
	VkDescriptorPool descriptorPool{ VK_NULL_HANDLE };
	VkDescriptorSetLayout descriptorSetLayoutTex{ VK_NULL_HANDLE };
	VkDescriptorSet descriptorSetTex{ VK_NULL_HANDLE };
#endif
	
private:
	void _test_LoadAssets();
	void _test_WaitForPreviousFrame();
	
	void _createSwapChain();
	void _recreateSwapChain();
	void _createBackBuffers(const Vec2i& frameSize);
	
	AXE_VkDevice*				   _dev = nullptr;
	AXE_VkSurfaceKHR			   _surface_vk;
	AXE_VkDeviceQueue              _graphQueue_vk;
//	AXE_VkDeviceQueue              _presentQueue_vk; no use atm, just simple use graphQuene to present
	AXE_VkSwapchainKHR             _swapChain_vk;
//	Vector<UPtr<BackBuffer_Vk>, 4> _backBuffers_vk;
	
	::HWND							_hwnd;
	
	static constexpr u32 kRenderTimerId = 100;
}; // Context_Vk_Win32

} // namespace axe

#endif // AXE_RENDER_HAS_VK
#endif // AXE_OS_WINDOWS