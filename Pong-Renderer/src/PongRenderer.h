#pragma once

#define PONG_RENDERER_API              __declspec(dllexport)

#include <Windows.h>

class ID3D11Device;
class ID3D11DeviceContext;
class ID3D11VertexShader;
class ID3D11PixelShader;
class ID3D11InputLayout;
class ID3D11Buffer;
class ID3D11Texture2D;
class ID3D11ShaderResourceView;
class ID3D11SamplerState;
class IDXGISwapChain;
class ID3D11RenderTargetView;
class ID3D10Blob;
class ID3D11RasterizerState;
class ID3D11BlendState;

namespace pong
{
	typedef unsigned int    uint;


	class PONG_RENDERER_API Renderer
	{
		struct VERTEX
		{
			float X;
			float Y;
			float TexX;
			float TexY;
		};

		static ID3D10Blob* CompileShader(const char* src, const char* version, const char* main);

	private:
		const uint                m_Width;
		const uint                m_Height;
		HWND                      m_Window;
		char const*               m_ClassName;

		//Direct X stuff
		ID3D11Device*             m_Dev;
		ID3D11DeviceContext*      m_DevCon;
		IDXGISwapChain*           m_SwapChain;
		ID3D11RasterizerState*    m_RasterizerState;
		ID3D11BlendState*         m_BlendState;

		ID3D11Texture2D*          m_BackBuffer;
		ID3D11RenderTargetView*   m_BackBufferView;

		ID3D11VertexShader*       m_VertexShader;
		ID3D11PixelShader*        m_PixelShader;
		ID3D11InputLayout*        m_InputLayout;

		//vertex buffer
		ID3D11Buffer*             m_VertexBuffer;

		//texture
		uint                      m_TexWidth;
		uint                      m_TexHeight;
		ID3D11Texture2D*          m_Texture;
		ID3D11ShaderResourceView* m_TextureResourceView;
		ID3D11SamplerState*       m_TextureSampler;

	public:
		Renderer(uint width, uint height, char const* className);
		~Renderer();
	private:
		void initDXTextrue();
		void initDXShaders();
		void initVertexBuffer();
		void initDX();
		void initHWND();
		void init();
	public:
		void drawTexture() const;
		void loadPixels(char* data);
		void swapBuffers() const;

		bool updateWindow();
	};
}