#include <d3d11.h>
#include <tchar.h>

#include <iostream>

#include "gui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

constexpr auto WIDTH = 1280;
constexpr auto HEIGHT = 800;
constexpr auto TITLEBAR_HEIGHT = 26;

static ID3D11Device* d3d_device = nullptr;
static ID3D11DeviceContext* d3d_device_context = nullptr;
static IDXGISwapChain* dxgi_swapchain = nullptr;
static UINT resize_width = 0;
static UINT resize_height = 0;
static ID3D11RenderTargetView* render_target = nullptr;
static POINTS window_pos;
static WNDCLASSEXW window_class;
static HWND window_handle;

bool create_device_d3d(HWND hWnd);
void cleanup_device_d3d();
void create_render_target();
void cleanup_render_target();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace DMA::GUI {
	bool init(LPCWSTR title) {
		ImGui_ImplWin32_EnableDpiAwareness();

		window_class.cbSize = sizeof(WNDCLASSEXW);
		window_class.style = CS_CLASSDC;
		window_class.lpfnWndProc = WndProc;
		window_class.cbClsExtra = 0L;
		window_class.cbWndExtra = 0L;
		window_class.hInstance = GetModuleHandle(nullptr);
		window_class.hIcon = nullptr;
		window_class.hCursor = nullptr;
		window_class.hbrBackground = nullptr;
		window_class.lpszMenuName = nullptr;
		window_class.lpszClassName = title;
		window_class.hIconSm = nullptr;
		::RegisterClassExW(&window_class);

		window_handle = ::CreateWindowW(
			window_class.lpszClassName,
			title,
			WS_POPUP,	// WS_OVERLAPPEDWINDOW
			100,
			100,
			WIDTH,
			HEIGHT,
			nullptr,
			nullptr,
			window_class.hInstance,
			nullptr
		);

		if (!create_device_d3d(window_handle)) {
			cleanup_device_d3d();
			::UnregisterClassW(window_class.lpszClassName, window_class.hInstance);
			return false;
		}

		::ShowWindow(window_handle, SW_SHOWDEFAULT);
		::UpdateWindow(window_handle);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_ViewportsEnable;
		io.ConfigWindowsMoveFromTitleBarOnly = true;
		io.IniFilename = nullptr;
		io.LogFilename = nullptr;

		ImGui::StyleColorsDark();
		io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 20.0f);

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		ImGui_ImplWin32_Init(window_handle);
		ImGui_ImplDX11_Init(d3d_device, d3d_device_context);

		return true;
	}

	bool begin(void) {
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				is_active = false;
		}

		if (!is_active)
			return false;

		if (resize_width != 0 && resize_height != 0) {
			cleanup_render_target();
			dxgi_swapchain->ResizeBuffers(0, resize_width, resize_height, DXGI_FORMAT_UNKNOWN, 0);
			resize_width = resize_height = 0;
			create_render_target();
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
		ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);

		return true;
	}

	void end(void) {
		ImGui::Render();
		d3d_device_context->OMSetRenderTargets(1, &render_target, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		dxgi_swapchain->Present(1, 0);
	}

	void destroy(void) {
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImPlot::DestroyContext();
		ImGui::DestroyContext();

		cleanup_device_d3d();
		::DestroyWindow(window_handle);
		::UnregisterClassW(window_class.lpszClassName, window_class.hInstance);
	}

	bool open_file(LPWSTR file_name, DWORD file_name_length, LPCWSTR filter) {
		OPENFILENAMEW ofn;

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = window_handle;
		ofn.lpstrFile = file_name;
		ofn.nMaxFile = file_name_length;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = nullptr;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		return GetOpenFileName(&ofn);
	}
}

bool create_device_d3d(HWND hWnd) {
	DXGI_SWAP_CHAIN_DESC dxgi;
	ZeroMemory(&dxgi, sizeof(dxgi));
	dxgi.BufferCount = 2;
	dxgi.BufferDesc.Width = 0;
	dxgi.BufferDesc.Height = 0;
	dxgi.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgi.BufferDesc.RefreshRate.Numerator = 60;
	dxgi.BufferDesc.RefreshRate.Denominator = 1;
	dxgi.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	dxgi.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgi.OutputWindow = hWnd;
	dxgi.SampleDesc.Count = 1;
	dxgi.SampleDesc.Quality = 0;
	dxgi.Windowed = TRUE;
	dxgi.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT create_device_flags = 0;
	D3D_FEATURE_LEVEL feature_level;
	const D3D_FEATURE_LEVEL feature_level_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, create_device_flags, feature_level_array, 2, D3D11_SDK_VERSION, &dxgi, &dxgi_swapchain, &d3d_device, &feature_level, &d3d_device_context);
	if (res == DXGI_ERROR_UNSUPPORTED)
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, create_device_flags, feature_level_array, 2, D3D11_SDK_VERSION, &dxgi, &dxgi_swapchain, &d3d_device, &feature_level, &d3d_device_context);
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
	case WM_LBUTTONDOWN:
		window_pos = MAKEPOINTS(lParam);
		return 0;
	case WM_MOUSEMOVE:
		if (wParam == MK_LBUTTON) {
			const auto points = MAKEPOINTS(lParam);
			auto rect = ::RECT{};
			::GetWindowRect(hWnd, &rect);
			rect.left += points.x - window_pos.x;
			rect.top += points.y - window_pos.y;

			if (window_pos.x >= 0 &&
				window_pos.x <= WIDTH &&
				window_pos.y >= 0 &&
				window_pos.y <= TITLEBAR_HEIGHT) {
				SetWindowPos(
					hWnd,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0, 0,
					SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOZORDER
				);
			}
		}
		return 0;
	}

	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}