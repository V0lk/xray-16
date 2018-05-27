#include "stdafx.h"

#include "ModuleLookup.hpp"

namespace XRay
{
ModuleHandle::ModuleHandle(const bool dontUnload) : handle(nullptr), dontUnload(dontUnload) {}

ModuleHandle::ModuleHandle(pcstr moduleName, bool dontUnload /*= false*/) : handle(nullptr), dontUnload(dontUnload)
{
    Open(moduleName);
}

ModuleHandle::~ModuleHandle()
{
    Сlose();
}

void* ModuleHandle::Open(pcstr moduleName)
{
    if (IsLoaded())
        Сlose();
    
    Log("Loading DLL:", moduleName);

#ifdef WINDOWS
    handle = LoadLibraryA(moduleName);
#elif defined(LINUX)
    handle = dlopen(name, RTLD_LAZY);
#endif
    if (handle == nullptr)
    {
#ifdef WINDOWS
        Msg("! Failed to load DLL: 0x%d", GetLastError());
#elif defined(LINUX)
        Msg("! Failed to load DLL: 0x%d", dlerror());
#endif
    }

    return handle;
}

void ModuleHandle::Сlose()
{
    if (dontUnload)
        return;

    bool closed = false;

#ifdef WINDOWS
    closed = FreeLibrary(static_cast<HMODULE>(handle)) != 0;
#else
    closed = dlclose(handle) == 0;
#endif

    if (closed == false)
    {
#ifdef WINDOWS
        Msg("! Failed to close DLL: 0x%d", GetLastError());
#elif LINUX
        Msg("! Failed to close DLL: 0x%d", dlerror());
#endif
    }

    handle = nullptr;
}

bool ModuleHandle::IsLoaded() const
{
    return handle != nullptr;
}

void* ModuleHandle::operator()() const
{
    return handle;
}

void* ModuleHandle::GetProcAddress(pcstr procName) const
{
    void* proc = nullptr;

#ifdef WINDOWS
    proc = ::GetProcAddress(static_cast<HMODULE>(handle), procName);
#elif defined(LINUX)
    proc = dlsym(handle, procedure);
#endif

    if (proc == nullptr)
    {
#ifdef WINDOWS
        Msg("! Failed to load procedure [%s] from DLL: 0x%d", procName, GetLastError());
#elif LINUX
        Msg("! Failed to load procedure [%s] from DLL: 0x%d", procName, dlerror());
#endif
    }

    return proc;
}
} // namespace XRay
