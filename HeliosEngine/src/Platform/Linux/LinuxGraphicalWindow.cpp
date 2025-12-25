#include "LinuxGraphicalWindow.h"

#ifdef HELIOS_PLATFORM_LINUX

#include <Helios/Utils/ShowMessage.h>
#include <GLFW/glfw3native.h> // needed for glfwGetX11Display, glfwGetX11Window, etc.

#define GLFW_RESIZE_NWSE_CURSOR 0x00036007
#define GLFW_RESIZE_NESW_CURSOR 0x00036008
#define GLFW_RESIZE_EW_CURSOR   0x00036005
#define GLFW_RESIZE_NS_CURSOR   0x00036006
#define GLFW_ARROW_CURSOR       0x00036001


#define _NET_WM_MOVERESIZE_SIZE_TOPLEFT 0
#define _NET_WM_MOVERESIZE_SIZE_TOP 1
#define _NET_WM_MOVERESIZE_SIZE_TOPRIGHT 2
#define _NET_WM_MOVERESIZE_SIZE_RIGHT 3
#define _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT 4
#define _NET_WM_MOVERESIZE_SIZE_BOTTOM 5
#define _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT 6
#define _NET_WM_MOVERESIZE_SIZE_LEFT 7
#define _NET_WM_MOVERESIZE_MOVE 8
#define _NET_WM_MOVERESIZE_SIZE_KEYBOARD 9
#define _NET_WM_MOVERESIZE_MOVE_KEYBOARD 10
#define _NET_WM_MOVERESIZE_CANCEL 11

namespace Helios::Internal
{

// X11 helper
static void StartWindowMove_X11(GLFWwindow* glfwWindow)
{
    Display* display = glfwGetX11Display();
    ::Window x11Window = glfwGetX11Window(glfwWindow);

    if (!display || !x11Window)
        return;

    // query current pointr position relative to root
    ::Window root_return, child_return;
    int rootX, rootY, winX, winY;
    unsigned int mask_return;

    XQueryPointer(display, x11Window, &root_return, &child_return,
                  &rootX, &rootY, &winX, &winY, &mask_return);

    Atom net_wm_move_resize = XInternAtom(display, "_NET_WM_MOVERESIZE", False);
    static constexpr long None = 0L;
    if (net_wm_move_resize == None)
        return;

    long data[5];
    data[0] = rootX;
    data[1] = rootY;
    data[2] = _NET_WM_MOVERESIZE_MOVE;
    data[3] = 1; // left mouse button
    data[4] = 0;

    XEvent ev = {};
    ev.xclient.type = ClientMessage;
    ev.xclient.window = x11Window;
    ev.xclient.message_type = net_wm_move_resize;
    ev.xclient.format = 32;
    memcpy(ev.xclient.data.l, data, sizeof(data));

    XSendEvent(display, DefaultRootWindow(display), False,
               SubstructureRedirectMask | SubstructureNotifyMask,
               &ev);

    XFlush(display);
}

static void StartWindowResize_X11(GLFWwindow* glfwWindow, int direction)
{
    Display* display = glfwGetX11Display();
    ::Window x11Window = glfwGetX11Window(glfwWindow);

    if (!display || !x11Window)
        return;

    ::Window root_return, child_return;
    int rootX, rootY, winX, winY;
    unsigned int mask_return;

    XQueryPointer(display, x11Window, &root_return, &child_return,
                  &rootX, &rootY, &winX, &winY, &mask_return);

    Atom net_wm_move_resize = XInternAtom(display, "_NET_WM_MOVERESIZE", False);
    if (net_wm_move_resize == 0)
        return;

    long data[5];
    data[0] = rootX;
    data[1] = rootY;
    data[2] = direction; // one of the constants
    data[3] = 1;   // left mouse button
    data[4] = 0;

    XEvent ev = {};
    ev.xclient.type = ClientMessage;
    ev.xclient.window = x11Window;
    ev.xclient.message_type = net_wm_move_resize;
    ev.xclient.format = 32;
    memcpy(ev.xclient.data.l, data, sizeof(data));

    XSendEvent(display, DefaultRootWindow(display), False,
               SubstructureRedirectMask | SubstructureNotifyMask,
               &ev);

    XFlush(display);
}


void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    LinuxGraphicalWindow* win = (LinuxGraphicalWindow*)glfwGetWindowUserPointer(window);
    if (!win)
        return;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        constexpr int titleBarHeight = 35;
        if (mouseY <= titleBarHeight && !ImGui::IsAnyItemHovered())
        {
            // diff between doubl;e click
            static double lastClickTime = 0.0;
            double currentTime = glfwGetTime();
            double delta = currentTime - lastClickTime;
            lastClickTime = currentTime;

            if (delta < 0.3) // double click threshold in seconds
            {
                if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED))
                    glfwRestoreWindow(window);
                else
                    glfwMaximizeWindow(window);

                return;
            }

            // move window
            int platform = glfwGetPlatform();
            if (platform == GLFW_PLATFORM_X11)
            {
                StartWindowMove_X11(window);
            }
            else if (platform == GLFW_PLATFORM_WAYLAND)
            {
                // wayland compositor should handle it
            }

            win->dragging = true;
            win->dragOffsetX = mouseX;
            win->dragOffsetY = mouseY;
            return;
        }

        if (win->currentResizeEdge != LinuxGraphicalWindow::ResizeEdge::None)
        {
            int platform = glfwGetPlatform();
            if (platform == GLFW_PLATFORM_X11)
            {
                int direction = 0;
                switch (win->currentResizeEdge)
                {
                    case LinuxGraphicalWindow::ResizeEdge::TopLeft:     direction = _NET_WM_MOVERESIZE_SIZE_TOPLEFT; break;
                    case LinuxGraphicalWindow::ResizeEdge::Top:         direction = _NET_WM_MOVERESIZE_SIZE_TOP; break;
                    case LinuxGraphicalWindow::ResizeEdge::TopRight:    direction = _NET_WM_MOVERESIZE_SIZE_TOPRIGHT; break;
                    case LinuxGraphicalWindow::ResizeEdge::Right:       direction = _NET_WM_MOVERESIZE_SIZE_RIGHT; break;
                    case LinuxGraphicalWindow::ResizeEdge::BottomRight: direction = _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT; break;
                    case LinuxGraphicalWindow::ResizeEdge::Bottom:      direction = _NET_WM_MOVERESIZE_SIZE_BOTTOM; break;
                    case LinuxGraphicalWindow::ResizeEdge::BottomLeft:  direction = _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT; break;
                    case LinuxGraphicalWindow::ResizeEdge::Left:        direction = _NET_WM_MOVERESIZE_SIZE_LEFT; break;
                    default: break;
                }
                StartWindowResize_X11(window, direction);
            }
            else if (platform == GLFW_PLATFORM_WAYLAND)
            {
                // compositor handles resizing
            }
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        win->dragging = false;
        win->resizing = false;
    }
}



void CursorPosCallback(GLFWwindow* window, double mouseX, double mouseY)
{
    LinuxGraphicalWindow* win = (LinuxGraphicalWindow*)glfwGetWindowUserPointer(window);
    if (!win) return;

    if (win->resizing)
    {
        int winX, winY;
        glfwGetWindowPos(window, &winX, &winY);

        int newWinX = winX;
        int newWinY = winY;
        int newWidth = win->resizeStartWinWidth;
        int newHeight = win->resizeStartWinHeight;

        double deltaX = mouseX - win->resizeDragStartX;
        double deltaY = mouseY - win->resizeDragStartY;

        switch (win->currentResizeEdge)
        {
            case LinuxGraphicalWindow::ResizeEdge::Left:
                newWinX = winX + (int)deltaX;
                newWidth = win->resizeStartWinWidth - (int)deltaX;
                break;
            case LinuxGraphicalWindow::ResizeEdge::Right:
                newWidth = win->resizeStartWinWidth + (int)deltaX;
                break;
            case LinuxGraphicalWindow::ResizeEdge::Top:
                newWinY = winY + (int)deltaY;
                newHeight = win->resizeStartWinHeight - (int)deltaY;
                break;
            case LinuxGraphicalWindow::ResizeEdge::Bottom:
                newHeight = win->resizeStartWinHeight + (int)deltaY;
                break;
            case LinuxGraphicalWindow::ResizeEdge::TopLeft:
                newWinX = winX + (int)deltaX;
                newWidth = win->resizeStartWinWidth - (int)deltaX;
                newWinY = winY + (int)deltaY;
                newHeight = win->resizeStartWinHeight - (int)deltaY;
                break;
            case LinuxGraphicalWindow::ResizeEdge::TopRight:
                newWidth = win->resizeStartWinWidth + (int)deltaX;
                newWinY = winY + (int)deltaY;
                newHeight = win->resizeStartWinHeight - (int)deltaY;
                break;
            case LinuxGraphicalWindow::ResizeEdge::BottomLeft:
                newWinX = winX + (int)deltaX;
                newWidth = win->resizeStartWinWidth - (int)deltaX;
                newHeight = win->resizeStartWinHeight + (int)deltaY;
                break;
            case LinuxGraphicalWindow::ResizeEdge::BottomRight:
                newWidth = win->resizeStartWinWidth + (int)deltaX;
                newHeight = win->resizeStartWinHeight + (int)deltaY;
                break;
            default:
                break;
        }

        if (newWidth < 100) newWidth = 100;
        if (newHeight < 100) newHeight = 100;

        glfwSetWindowPos(win->m_Window, newWinX, newWinY);
        glfwSetWindowSize(win->m_Window, newWidth, newHeight);
        return;
    }

    // resize hints
    constexpr int borderThickness = 8;
    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);

    bool onLeft = mouseX >= 0 && mouseX <= borderThickness;
    bool onRight = mouseX >= winWidth - borderThickness && mouseX <= winWidth;
    bool onTop = mouseY >= 0 && mouseY <= borderThickness;
    bool onBottom = mouseY >= winHeight - borderThickness && mouseY <= winHeight;

    LinuxGraphicalWindow::ResizeEdge edge = LinuxGraphicalWindow::ResizeEdge::None;
    if (onLeft && onTop) edge = LinuxGraphicalWindow::ResizeEdge::TopLeft;
    else if (onLeft && onBottom) edge = LinuxGraphicalWindow::ResizeEdge::BottomLeft;
    else if (onRight && onTop) edge = LinuxGraphicalWindow::ResizeEdge::TopRight;
    else if (onRight && onBottom) edge = LinuxGraphicalWindow::ResizeEdge::BottomRight;
    else if (onLeft) edge = LinuxGraphicalWindow::ResizeEdge::Left;
    else if (onRight) edge = LinuxGraphicalWindow::ResizeEdge::Right;
    else if (onTop) edge = LinuxGraphicalWindow::ResizeEdge::Top;
    else if (onBottom) edge = LinuxGraphicalWindow::ResizeEdge::Bottom;

    win->currentResizeEdge = edge;

    GLFWcursor* cursor = win->arrowCursor;

    switch (edge)
    {
        case LinuxGraphicalWindow::ResizeEdge::TopLeft:
        case LinuxGraphicalWindow::ResizeEdge::BottomRight:
            cursor = win->resizeNWSE; break;
        case LinuxGraphicalWindow::ResizeEdge::TopRight:
        case LinuxGraphicalWindow::ResizeEdge::BottomLeft:
            cursor = win->resizeNESW; break;
        case LinuxGraphicalWindow::ResizeEdge::Left:
        case LinuxGraphicalWindow::ResizeEdge::Right:
            cursor = win->resizeEW; break;
        case LinuxGraphicalWindow::ResizeEdge::Top:
        case LinuxGraphicalWindow::ResizeEdge::Bottom:
            cursor = win->resizeNS; break;
        default:
            cursor = win->arrowCursor; break;
    }

    glfwSetCursor(window, cursor);

}

void LinuxGraphicalWindow::SetVSync(bool enabled)
{
    m_Context->SetVSync(enabled);
}

void LinuxGraphicalWindow::SetTitle(const std::string& title)
{
    glfwSetWindowTitle(m_Window, title.c_str());
}

Message::Result LinuxGraphicalWindow::ShowMessage(const std::string& title, const std::string& message, Message::Flags type) const
{
    return Helios::ShowMessage(title, message, type);
}

bool LinuxGraphicalWindow::VSyncEnabled() const {
    return m_Context->GetVSyncInterval();
}

} // namespace Helios::Internal

#endif
