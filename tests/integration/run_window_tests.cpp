#include "../support/run_checks.hpp"
#include "core/run_start.hpp"
#include "platform/windows/run_window.hpp"
#include <iostream>
using namespace closed_circuit;

int wmain(int argc, wchar_t **argv)
{
    if (argc != 2 || FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
        return 1;
    const TextRecord record{"title", "[AI PLACEHOLDER] Run input test",
                            TextStatus::ai_placeholder, std::nullopt};
    const auto catalog = *TextCatalog::from_records(std::span{&record, 1}, {},
                                                    TextAudience::developer);
    auto window = windows::NativeWindow::create(catalog, "title");
    if (!window.window)
        return 2;
    auto initial = start_run(RunSeed{0});
    // Numeric integration fixture, drawn only in our off-desktop test window.
    GameState sector{.player_position = *NodeId::from_value(0)};
    sector.ports[0] = *PortMask::from_bits(2);
    sector.ports[1] = *PortMask::from_bits(10);
    sector.ports[2] = *PortMask::from_bits(8);
    sector.rotatable_nodes[1] = true;
    sector.sector_entry = *NodeId::from_value(0);
    sector.sector_exit = *NodeId::from_value(48);
    sector.receiver_nodes[2] = true;
    sector.packet = Packet{*NodeId::from_value(0), *NodeId::from_value(2)};
    sector.pulses = {{*EntityId::from_value(9), *NodeId::from_value(1)}};
    initial.sector = sector;
    const auto original = initial;
    const AssetManifest assets{presentation::atlas_reference()};
    auto created =
        windows::RunWindow::create(*window.window, initial, argv[1], assets);
    if (!created.session)
        return 3;
    const auto hwnd = window.window->handle();
    SetWindowPos(hwnd, nullptr, -32000, -32000, 640, 480,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
    UpdateWindow(hwnd);
    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
    auto expected = initial;
    unsigned accepted = 0;
    const auto action = [&](WPARAM key, std::optional<Command> command)
    {
        if (command)
        {
            auto next = resolve_run_turn(expected, *command);
            accepted += next.accepted;
            expected = std::move(next.state);
        }
        SendMessageW(hwnd, WM_KEYDOWN, key, 0);
        const auto matches = [&]
        {
            return test::same_run(created.session->state(), expected) &&
                   test::same_state(created.session->view().board(),
                                    expected.sector);
        };
        if (!matches())
            return false;
        // Both OS repeat and a duplicate down without bit30 must be ignored.
        for (int repeat = 0; repeat < 20; ++repeat)
        {
            SendMessageW(hwnd, WM_KEYDOWN, key, repeat % 2 ? (1LL << 30) : 0);
            if (!matches())
                return false;
        }
        UpdateWindow(hwnd);
        SendMessageW(hwnd, WM_KEYUP, key, 0);
        return matches() && created.session->error() == S_OK;
    };
    const auto node = [](unsigned value) { return *NodeId::from_value(value); };
    if (!action(VK_RIGHT, MoveCommand{node(1)}) ||
        !action('R', RotateCommand{node(1)}) ||
        !action('O', OverdriveCommand{node(1)}) ||
        !action(VK_SPACE, WaitCommand{}) ||
        !action(VK_LEFT, MoveCommand{node(0)}) ||
        !action(VK_UP, std::nullopt) ||
        !action(VK_DOWN, MoveCommand{node(8)}) ||
        !action(VK_RIGHT, MoveCommand{node(2)}) ||
        !action('R', RotateCommand{node(1)}) ||
        !action(VK_RIGHT, MoveCommand{node(2)}) || !action('A', std::nullopt))
        return 4;
    if (accepted != 6 || !test::same_run(initial, original) ||
        expected.sector.packet || !expected.sector.pulses.empty() ||
        !expected.sector.powered_nodes[2] ||
        expected.sector.player_integrity != 2)
        return 5;
    SendMessageW(hwnd, WM_KILLFOCUS, 0, 0);
    SendMessageW(hwnd, WM_KEYDOWN, VK_SPACE, 0);
    SendMessageW(hwnd, WM_KEYUP, VK_SPACE, 0);
    SendMessageW(hwnd, WM_SETFOCUS, 0, 0);
    SendMessageW(hwnd, WM_KEYDOWN, VK_SPACE, 1LL << 30);
    SendMessageW(hwnd, WM_KEYUP, VK_SPACE, 0);
    if (!test::same_run(created.session->state(), expected))
        return 6;
    for (unsigned i = 0; i < 50; ++i)
        if (!action(VK_SPACE, WaitCommand{}))
            return 7;
    if (expected.sector.turns_taken != TurnIndex{48} ||
        run_phase(expected) != RunPhase::lost ||
        !action('R', RotateCommand{node(2)}) ||
        !action('O', OverdriveCommand{node(2)}) ||
        !action(VK_LEFT, MoveCommand{node(1)}))
        return 8;
    SendMessageW(hwnd, WM_CLOSE, 0, 0);
    created.session->key_available();
    if (!test::same_run(created.session->state(), expected))
        return 9;
    created.session.reset();
    return 0;
}
