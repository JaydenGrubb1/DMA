#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <cstdlib>

static ID3D11Device* d3d_device = nullptr;
static ID3D11DeviceContext* d3d_device_context = nullptr;
static IDXGISwapChain* dxgi_swapchain = nullptr;
static UINT resize_width = 0;
static UINT resize_height = 0;
static ID3D11RenderTargetView* render_target = nullptr;

bool create_device_d3d(HWND hWnd);
void cleanup_device_d3d();
void create_render_target();
void cleanup_render_target();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);
	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Digital Music Analyzer", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

	// Initialize Direct3D
	if (!create_device_d3d(hwnd)) {
		cleanup_device_d3d();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return EXIT_FAILURE;
	}

	// Show window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Intialize ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;

	// Configure ImGui style
	ImGui::StyleColorsDark();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//ImGui::StyleColorsLight();

	// Initialize renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(d3d_device, d3d_device_context);

	// Main loop
	bool done = false;
	while (!done) {
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}

		if (done)
			break;

		// Handle window resize
		if (resize_width != 0 && resize_height != 0) {
			cleanup_render_target();
			dxgi_swapchain->ResizeBuffers(0, resize_width, resize_height, DXGI_FORMAT_UNKNOWN, 0);
			resize_width = resize_height = 0;
			create_render_target();
		}

		// Start GUI frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// TODO: Add actual GUI here
		ImGui::ShowDemoWindow(nullptr);

		// Render GUI
		ImGui::Render();
		const float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
		d3d_device_context->OMSetRenderTargets(1, &render_target, nullptr);
		d3d_device_context->ClearRenderTargetView(render_target, clear_color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		dxgi_swapchain->Present(1, 0);
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	cleanup_device_d3d();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);

	return EXIT_SUCCESS;
}

bool create_device_d3d(HWND hWnd) {
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT create_device_flags = 0;
	D3D_FEATURE_LEVEL feature_level;
	const D3D_FEATURE_LEVEL feature_level_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, create_device_flags, feature_level_array, 2, D3D11_SDK_VERSION, &sd, &dxgi_swapchain, &d3d_device, &feature_level, &d3d_device_context);
	if (res == DXGI_ERROR_UNSUPPORTED)
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, create_device_flags, feature_level_array, 2, D3D11_SDK_VERSION, &sd, &dxgi_swapchain, &d3d_device, &feature_level, &d3d_device_context);
	if (res != S_OK)
		return false;

	create_render_target();
	return true;
}

void cleanup_device_d3d() {
	cleanup_render_target();
	if (dxgi_swapchain) {
		dxgi_swapchain->Release();
		dxgi_swapchain = nullptr;
	}
	if (d3d_device_context) {
		d3d_device_context->Release();
		d3d_device_context = nullptr;
	}
	if (d3d_device) {
		d3d_device->Release();
		d3d_device = nullptr;
	}
}

void create_render_target() {
	ID3D11Texture2D* back_buffer;
	dxgi_swapchain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));

	if (back_buffer) {
		d3d_device->CreateRenderTargetView(back_buffer, nullptr, &render_target);
		back_buffer->Release();
	}
}

void cleanup_render_target() {
	if (render_target) {
		render_target->Release();
		render_target = nullptr;
	}
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg) {
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		resize_width = (UINT)LOWORD(lParam);
		resize_height = (UINT)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}