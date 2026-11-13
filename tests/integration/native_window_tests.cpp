#include "platform/windows/native_window.hpp"
#include <iostream>
#include <type_traits>
using namespace closed_circuit;

static_assert(!std::is_copy_constructible_v<windows::NativeWindow>);
static_assert(!std::is_move_constructible_v<windows::NativeWindow>);

std::wstring class_name(HWND window)
{
    wchar_t name[256]{};
    if (!GetClassNameW(window, name, 256))
        return {};
    return name;
}
bool registered(const std::wstring &name)
{
    WNDCLASSEXW info{};
    info.cbSize = sizeof(info);
    return GetClassInfoExW(GetModuleHandleW(nullptr), name.c_str(), &info) != 0;
}

int main()
{
    const TextRecord record{"title", "[AI PLACEHOLDER] Test \xc5\xbc",
                            TextStatus::ai_placeholder, std::nullopt};
    const auto catalog = *TextCatalog::from_records(std::span{&record, 1}, {},
                                                    TextAudience::developer);
    const auto unknown = windows::NativeWindow::create(catalog, "unknown");
    if (unknown.window || unknown.error != ERROR_NOT_FOUND)
        return 1;
    auto bad = record;
    bad.value += '\xff';
    const auto invalid_catalog = *TextCatalog::from_records(
        std::span{&bad, 1}, {}, TextAudience::developer);
    const auto invalid =
        windows::NativeWindow::create(invalid_catalog, "title");
    if (invalid.window || invalid.error != ERROR_NO_UNICODE_TRANSLATION)
        return 1;
    for (unsigned repeat = 0; repeat < 4; ++repeat)
    {
        auto first = windows::NativeWindow::create(catalog, "title");
        auto second = windows::NativeWindow::create(catalog, "title");
        if (!first.window || !second.window || first.error != ERROR_SUCCESS ||
            second.error != ERROR_SUCCESS)
        {
            std::cerr << "Window creation failed: " << first.error << ", "
                      << second.error << '\n';
            return 1;
        }
        const auto one = first.window->handle();
        const auto two = second.window->handle();
        const auto first_class = class_name(one),
                   second_class = class_name(two);
        wchar_t title[256]{};
        if (!IsWindow(one) || !IsWindow(two) || one == two ||
            IsWindowVisible(one) || IsWindowVisible(two) ||
            first_class.empty() || second_class.empty() ||
            first_class == second_class || !registered(first_class) ||
            !registered(second_class) || !GetWindowTextW(one, title, 256) ||
            std::wstring(title) != L"[AI PLACEHOLDER] Test \u017c")
            return 1;
        if (repeat % 2 == 0)
        {
            SendMessageW(one, WM_CLOSE, 0, 0);
            if (IsWindow(one) || first.window->handle() != nullptr)
                return 1;
        }
        first.window.reset();
        if (IsWindow(one) || registered(first_class) || !IsWindow(two) ||
            !registered(second_class))
            return 1;
        second.window.reset();
        if (IsWindow(two) || registered(second_class))
            return 1;
    }
    return 0;
}
