#include <d3d11.h>
#include <tchar.h>

#include <iostream>

#include "gui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

constexpr auto WIDTH = 1280;
constexpr auto HEIGHT = 800;
constexpr auto TITLEBAR_HEIGHT = 26;

static ID3D11Device* _d3d_device = nullptr;
static ID3D11DeviceContext* _d3d_device_context = nullptr;
static IDXGISwapChain* _dxgi_swapchain = nullptr;
static UINT _resize_width = 0;
static UINT _resize_height = 0;
static ID3D11RenderTargetView* _render_target = nullptr;
static POINTS _window_pos;
static WNDCLASSEXW _window_class;
static HWND _window_handle;

bool __create_device_d3d(HWND hWnd);
void __cleanup_device_d3d();
void __create_render_target();
void __cleanup_render_target();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace DMA::GUI {
	bool init(LPCWSTR title) {
		ImGui_ImplWin32_EnableDpiAwareness();

		_window_class.cbSize = sizeof(WNDCLASSEXW);
		_window_class.style = CS_CLASSDC;
		_window_class.lpfnWndProc = WndProc;
		_window_class.cbClsExtra = 0L;
		_window_class.cbWndExtra = 0L;
		_window_class.hInstance = GetModuleHandle(nullptr);
		_window_class.hIcon = nullptr;
		_window_class.hCursor = nullptr;
		_window_class.hbrBackground = nullptr;
		_window_class.lpszMenuName = nullptr;
		_window_class.lpszClassName = title;
		_window_class.hIconSm = nullptr;
		::RegisterClassExW(&_window_class);

		_window_handle = ::CreateWindowW(
			_window_class.lpszClassName,
			title,
			WS_POPUP,	// WS_OVERLAPPEDWINDOW
			100,
			100,
			WIDTH,
			HEIGHT,
			nullptr,
			nullptr,
			_window_class.hInstance,
			nullptr
		);

		if (!__create_device_d3d(_window_handle)) {
			__cleanup_device_d3d();
			::UnregisterClassW(_window_class.lpszClassName, _window_class.hInstance);
			return false;
		}

		::ShowWindow(_window_handle, SW_SHOWDEFAULT);
		::UpdateWindow(_window_handle);

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

		ImGui_ImplWin32_Init(_window_handle);
		ImGui_ImplDX11_Init(_d3d_device, _d3d_device_context);

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

		if (_resize_width != 0 && _resize_height != 0) {
			__cleanup_render_target();
			_dxgi_swapchain->ResizeBuffers(0, _resize_width, _resize_height, DXGI_FORMAT_UNKNOWN, 0);
			_resize_width = _resize_height = 0;
			__create_render_target();
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
		_d3d_device_context->OMSetRenderTargets(1, &_render_target, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		_dxgi_swapchain->Present(1, 0);
	}

	void destroy(void) {
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImPlot::DestroyContext();
		ImGui::DestroyContext();

		__cleanup_device_d3d();
		::DestroyWindow(_window_handle);
		::UnregisterClassW(_window_class.lpszClassName, _window_class.hInstance);
	}

	bool open_file(LPWSTR file_name, DWORD file_name_length, LPCWSTR filter) {
		OPENFILENAMEW ofn;

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = _window_handle;
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

bool __create_device_d3d(HWND hWnd) {
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
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, create_device_flags, feature_level_array, 2, D3D11_SDK_VERSION, &dxgi, &_dxgi_swapchain, &_d3d_device, &feature_level, &_d3d_device_context);
	if (res == DXGI_ERROR_UNSUPPORTED)
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, create_device_flags, feature_level_array, 2, D3D11_SDK_VERSION, &dxgi, &_dxgi_swapchain, &_d3d_device, &feature_level, &_d3d_device_context);
	if (res != S_OK)
		return false;

	__create_render_target();
	return true;
}

void __cleanup_device_d3d() {
	__cleanup_render_target();
	if (_dxgi_swapchain) {
		_dxgi_swapchain->Release();
		_dxgi_swapchain = nullptr;
	}
	if (_d3d_device_context) {
		_d3d_device_context->Release();
		_d3d_device_context = nullptr;
	}
	if (_d3d_device) {
		_d3d_device->Release();
		_d3d_device = nullptr;
	}
}

void __create_render_target() {
	ID3D11Texture2D* back_buffer;
	_dxgi_swapchain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));

	if (back_buffer) {
		_d3d_device->CreateRenderTargetView(back_buffer, nullptr, &_render_target);
		back_buffer->Release();
	}
}

void __cleanup_render_target() {
	if (_render_target) {
		_render_target->Release();
		_render_target = nullptr;
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
		_resize_width = (UINT)LOWORD(lParam);
		_resize_height = (UINT)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	case WM_LBUTTONDOWN:
		_window_pos = MAKEPOINTS(lParam);
		return 0;
	case WM_MOUSEMOVE:
		if (wParam == MK_LBUTTON) {
			const auto points = MAKEPOINTS(lParam);
			auto rect = ::RECT{};
			::GetWindowRect(hWnd, &rect);
			rect.left += points.x - _window_pos.x;
			rect.top += points.y - _window_pos.y;

			if (_window_pos.x >= 0 &&
				_window_pos.x <= WIDTH &&
				_window_pos.y >= 0 &&
				_window_pos.y <= TITLEBAR_HEIGHT) {
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