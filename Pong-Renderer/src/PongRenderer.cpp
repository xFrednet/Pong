#include "PongRenderer.h"

#pragma warning(push, 0)
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <iostream>
#pragma warning(pop)

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "d3dcompiler.lib")
 
static const char* SHADER_SRC = R"(
/* //////////////////////////////////////////////////////////////////////////////// */
// ///Input/Output //
/* //////////////////////////////////////////////////////////////////////////////// */
struct ZA_VS_INPUT {
	float4 Position			: POSITION;
	float2 TexCoord			: TEX_COORD;
};

struct ZA_VS_OUTPUT {
	float4 Position			: SV_POSITION;
	float2 TexCoord			: TEX_COORD;
};

/* //////////////////////////////////////////////////////////////////////////////// */
// // Vertex Shader //
/* //////////////////////////////////////////////////////////////////////////////// */
ZA_VS_OUTPUT VSMain(ZA_VS_INPUT input)
{
	return input;
}

/* //////////////////////////////////////////////////////////////////////////////// */
// // Pixel Shader //
/* //////////////////////////////////////////////////////////////////////////////// */

Texture2D Texture				: register(t0);
SamplerState TextureSampler		: register(s0);

float4 PSMain(ZA_VS_OUTPUT input) : SV_TARGET
{
	float4 c = Texture.Sample(TextureSampler, input.TexCoord);
	return float4(c.xyz, 1.0f);
})";

static const D3D11_INPUT_ELEMENT_DESC SHADER_IED[]{
	{ "POSITION"	, 0, DXGI_FORMAT_R32G32_FLOAT	, 0, 0					, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEX_COORD"	, 0, DXGI_FORMAT_R32G32_FLOAT	, 0, sizeof(float) * 2	, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

namespace pong
{
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	ID3D10Blob* Renderer::CompileShader(const char* src, const char* version, const char* main)
	{
		ID3D10Blob *shaderBlob;
		ID3D10Blob *errorBlob;
		HRESULT result = D3DCompile(src, strlen(src), NULL, NULL, NULL, main, version, D3DCOMPILE_DEBUG, 0, &shaderBlob, &errorBlob);
		if (result == S_OK)
			return shaderBlob;
		else {
			std::cout << "compileShader: Unable to compile shader from source" << std::endl;
			if (errorBlob) {
				std::cout << "compileShader: targeted version: " << version << std::endl;

				if (errorBlob->GetBufferSize()) {
					std::cout << "compileShader: Errors: " << std::endl;
					std::cout << (const char*)errorBlob->GetBufferPointer() << std::endl;
				}
			}
			exit(-1);
		}
		return nullptr;
	}

	Renderer::Renderer(uint width, uint height)
		: m_Width(width),
		m_Height(height),
		m_Dev(nullptr), m_DevCon(nullptr),
		m_VertexShader(nullptr), m_PixelShader(nullptr),
		m_VertexBuffer(nullptr),
		m_Texture(nullptr), m_TextureResourceView(nullptr), m_TextureSampler(nullptr)
	{
		init();
	}

	Renderer::~Renderer()
	{
		ShowWindow(m_Window, SW_HIDE);
		DestroyWindow(m_Window);

		m_TextureSampler->Release();
		m_TextureResourceView->Release();
		m_Texture->Release();

		m_VertexBuffer->Release();

		m_PixelShader->Release();
		m_VertexShader->Release();

		m_BackBufferView->Release();
		m_BackBuffer->Release();

		m_SwapChain->Release();
		m_DevCon->Release();
		m_Dev->Release();
	}

	//
	// Init
	//
	void Renderer::initVertexBuffer()
	{
		HRESULT hr;
		VERTEX vertices[6];
		{
			float texW = float(m_Width) / float(m_TexWidth);
			float texH = float(m_Height) / float(m_TexHeight);
			// 0
			//
			// 1   2
			vertices[0] = { -1.0f,  1.0f, 0.0f, 0.0f };
			vertices[1] = { -1.0f,  -1.0f, 0.0f, texH };
			vertices[2] = { 1.0f, -1.0f, texW, texH };

			// 3   5
			//
			//     4
			vertices[3] = { -1.0f,  1.0f, 0.0f, 0.0f };
			vertices[4] = { 1.0f, -1.0f, texW, texH };
			vertices[5] = { 1.0f,  1.0f, texW, 0.0f };
		}


		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX) * 6;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		//create Buffer
		hr = m_Dev->CreateBuffer(&bd, NULL, &m_VertexBuffer);
		
		D3D11_MAPPED_SUBRESOURCE ms;
		hr = m_DevCon->Map(m_VertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, vertices, sizeof(VERTEX) * 6);
		m_DevCon->Unmap(m_VertexBuffer, NULL);
	}
	void Renderer::initDXTextrue()
	{
		HRESULT result;
		/* ********************************************************* */
		// * Texture *
		/* ********************************************************* */
		D3D11_TEXTURE2D_DESC desc;
		memset(&desc, 0, sizeof(D3D11_TEXTURE2D_DESC));

		desc.Width = m_TexWidth;
		desc.Height = m_TexHeight;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		result = m_Dev->CreateTexture2D(&desc, nullptr, &m_Texture);
		if (FAILED(result))
			exit(-6);

		/* ********************************************************* */
		// * ShaderResourceView *
		/* ********************************************************* */
		D3D11_SHADER_RESOURCE_VIEW_DESC resDesc;
		memset(&resDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		resDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		resDesc.Texture2D.MostDetailedMip = 0;
		resDesc.Texture2D.MipLevels = -1;

		result = m_Dev->CreateShaderResourceView(m_Texture, &resDesc, &m_TextureResourceView);
		if (FAILED(result))
			exit(-7);

		/* ********************************************************* */
		// * Sampler *
		/* ********************************************************* */
		D3D11_SAMPLER_DESC sampler_desc;

		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;// : D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.MipLODBias = 0;
		sampler_desc.MaxAnisotropy = 1;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.BorderColor[0] = 0;
		sampler_desc.BorderColor[1] = 0;
		sampler_desc.BorderColor[2] = 0;
		sampler_desc.BorderColor[3] = 0;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

		result = m_Dev->CreateSamplerState(&sampler_desc, &m_TextureSampler);
		if (FAILED(result))
			exit(-8);
	}
	void Renderer::initDXShaders()
	{
		ID3D10Blob *VS;
		ID3D10Blob *PS;

		VS = CompileShader(SHADER_SRC, "vs_4_0", "VSMain");
		PS = CompileShader(SHADER_SRC, "ps_4_0", "PSMain");

		//creation
		m_Dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_VertexShader);
		m_Dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_PixelShader);

		if (!m_VertexShader || !m_PixelShader)
			exit(-2);

		//setting the shaders
		m_DevCon->VSSetShader(m_VertexShader, nullptr, 0);
		m_DevCon->PSSetShader(m_PixelShader, nullptr, 0);

		//input layout
		{
			m_Dev->CreateInputLayout(SHADER_IED, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &m_InputLayout);
			if (!m_InputLayout) {
				exit(-3);
			}

			m_DevCon->IASetInputLayout(m_InputLayout);
		}

		//cleanup
		VS->Release();
		PS->Release();
	}
	void Renderer::initDX()
	{
		HRESULT hr;
		//
		// Swap chain
		//
		{
			UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#			ifdef _DEBUG
			creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#			endif

			DXGI_SWAP_CHAIN_DESC scd;
			ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

			scd.BufferCount = 1;
			scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			scd.BufferDesc.Width = UINT(m_Width);
			scd.BufferDesc.Height = UINT(m_Height);
			scd.BufferDesc.RefreshRate.Numerator = 60;
			scd.BufferDesc.RefreshRate.Denominator = 1;
			scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			scd.OutputWindow = m_Window;
			scd.SampleDesc.Count = 1;//TODO add option
			scd.SampleDesc.Quality = 0;
			scd.Windowed = true;
			scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			D3D_FEATURE_LEVEL FeatureLevels = D3D_FEATURE_LEVEL_11_0;
			D3D_FEATURE_LEVEL FeatureLevel;

			hr = D3D11CreateDeviceAndSwapChain(NULL,
				D3D_DRIVER_TYPE_HARDWARE,
				NULL,
				creationFlags,
				&FeatureLevels,
				1,
				D3D11_SDK_VERSION,
				&scd,
				&m_SwapChain,
				&m_Dev,
				&FeatureLevel,
				&m_DevCon);
		}

		//
		// Render target
		//
		{

			hr = m_SwapChain->ResizeBuffers(0, m_Width, m_Height, DXGI_FORMAT_UNKNOWN, 0);

			hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_BackBuffer);

			hr = m_Dev->CreateRenderTargetView(m_BackBuffer, nullptr, &m_BackBufferView);

			m_DevCon->OMSetRenderTargets(1, &m_BackBufferView, nullptr);
			m_DevCon->ClearRenderTargetView(m_BackBufferView, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			m_DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// ~ Rasterizer state ~
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		{
			D3D11_RASTERIZER_DESC rDesc;
			ZeroMemory(&rDesc, sizeof(D3D11_RASTERIZER_DESC));

			//draw options
			rDesc.FillMode = D3D11_FILL_WIREFRAME;
			rDesc.FillMode = D3D11_FILL_SOLID;
			rDesc.CullMode = D3D11_CULL_NONE;
			rDesc.FrontCounterClockwise = true;

			hr = m_Dev->CreateRasterizerState(&rDesc, &m_RasterizerState);
			if (FAILED(hr))
				return exit(-15);
		}
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// ~ Blend state ~
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		{
			D3D11_BLEND_DESC blendDesc;
			ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

			//option
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

			//creation
			hr = m_Dev->CreateBlendState(&blendDesc, &m_BlendState);

			//error check
			if (FAILED(hr))
				return exit(-16);
		}

		D3D11_VIEWPORT viewport;
		{
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;
			viewport.Width = (float)m_Width;
			viewport.Height = (float)m_Height;
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;

			m_DevCon->RSSetViewports(1, &viewport);
		}

		float blendFactor[4] = { 1.0f, 1.0f, 0.0f, 0.0f };

		/*m_DevCon->OMSetDepthStencilState(m_DepthStencilState, 1);*/
		m_DevCon->OMSetBlendState(m_BlendState, blendFactor, 0xffffffff);
		m_DevCon->RSSetState(m_RasterizerState);
		m_DevCon->RSSetViewports(1, &viewport);
	}
	void Renderer::initHWND()
	{
		WNDCLASSEX wc;
		DWORD style = WS_OVERLAPPEDWINDOW;

		memset(&wc, 0, sizeof(WNDCLASSEX));

		wc.cbSize        = sizeof(WNDCLASSEX);
		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpszClassName = RENDERER_CLASS_NAME;
		wc.lpfnWndProc   = WindowProc;

		RECT size = { 0, 0, LONG(m_Width), LONG(m_Height) };
		AdjustWindowRect(&size, style, false);

		RegisterClassEx(&wc);

		m_Window = CreateWindowEx(NULL,
			RENDERER_CLASS_NAME,
			RENDERER_WINDOW_TITLE,
			style,
			100,
			100,
			size.right - size.left,
			size.bottom - size.top,
			NULL, NULL, NULL, NULL);

		ShowWindow(m_Window, SW_SHOW);
	}
	void Renderer::init()
	{
		float log = log2f(max((float)m_Width, (float)m_Height));
		if (floor(log) != log)
			log = floor(log) + 1;
		m_TexHeight = m_TexWidth = (uint)powf(2.0f, log);

		initHWND();
		initDX();
		initDXShaders();
		initDXTextrue();
		initVertexBuffer();
	}

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message) {
			//
			// Focus
			//
		case WM_KILLFOCUS:
			//Input::ClearButtons();
			//Input::ClearKeys();
			break;

			//
			// Keyboard
			//
		case WM_KEYUP:
			//Input::KeyEvent(wParam, false);
			break;
		case WM_KEYDOWN:
			//Input::KeyEvent(wParam, true);
			break;
			//
			// Mouse
			//
		case WM_MOUSEMOVE:
			//Input::MouseMotionEvent(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;

			//LButton
		case WM_LBUTTONDOWN:
			break;
		case WM_LBUTTONUP:
			break;

			//MButton
		case WM_MBUTTONDOWN:
			break;
		case WM_MBUTTONUP:
			break;

			//RButton
		case WM_RBUTTONDOWN:
			break;
		case WM_RBUTTONUP:
			break;

			//
			// Resize
			//
		case WM_SIZE:
			break;

			//
			// Else
			//
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			break;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	void Renderer::drawTexture() const
	{

		m_DevCon->VSSetShader(m_VertexShader, nullptr, 0);
		m_DevCon->PSSetShader(m_PixelShader, nullptr, 0);

		m_DevCon->PSSetShaderResources(0, 1, &m_TextureResourceView);
		m_DevCon->PSSetSamplers(0, 1, &m_TextureSampler);

		uint stride = sizeof(VERTEX);
		uint offset = 0;
		m_DevCon->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
		m_DevCon->Draw(6, 0);
	}

	void Renderer::loadPixels(char* data)
	{
		D3D11_MAPPED_SUBRESOURCE ms;
		HRESULT h = m_DevCon->Map(m_Texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
		
		uintptr_t dest = (uintptr_t)ms.pData;
		uintptr_t source = (uintptr_t)data;
		for (uint i = 0; i < m_Height; i++)
		{
			memcpy((void*)dest, (void*)source, m_Width * 4);
			dest += m_TexWidth * 4;
			source += m_Width * 4;
		}
		m_DevCon->Unmap(m_Texture, 0);
	}

	void Renderer::swapBuffers() const
	{
		m_SwapChain->Present(0, 0);
	}

	MSG msg_;
	bool Renderer::updateWindow()
	{
		if (PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg_);
			DispatchMessage(&msg_);

			if (msg_.message == WM_QUIT)
				return false;
		}
		return true;
	}
}
